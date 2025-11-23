#!/usr/bin/env python3
"""
Offline Q-table trainer.

入力:  q_client が出力する JSONL (logs/experience.jsonl など)
出力:  Q テーブル (models/q_table.json) と学習メトリクス (runs/train_metrics.csv / .png)

各行は以下のいずれか:
- 行動行: {"ep":int,"t":int,"state":[...17...],"action_idx":int,"action_flat":[...120...],"reward":int,"done":0}
- 終端行: {"ep":int,"t":int,"final_reward":int,"done":1}
"""

import argparse
import csv
import json
import sys
import datetime
from collections import defaultdict
from pathlib import Path
from typing import Dict, Iterable, List


def load_jsonl(path: Path) -> Iterable[dict]:
    with path.open() as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            try:
                yield json.loads(line)
            except json.JSONDecodeError:
                continue


def action_key(action_flat: List[int]) -> str:
    # 非ゼロのみを位置:値で持つことでサイズ削減
    parts = [f"{i}:{v}" for i, v in enumerate(action_flat) if v != 0]
    return ";".join(parts)


def state_key(state_vec: List[int]) -> str:
    return ",".join(str(v) for v in state_vec)


def load_model(path: Path) -> Dict[str, Dict[str, float]]:
    if not path.exists():
        return {}
    try:
        with path.open() as f:
            return json.load(f)
    except Exception:
        return {}


def train(
    q_log: Path,
    init_model: Dict[str, Dict[str, float]],
    out_model: Path,
    metrics_csv: Path,
    metrics_png: Path,
    gamma: float,
    lr: float,
    epochs: int,
) -> None:
    episodes: Dict[int, List[dict]] = defaultdict(list)
    for row in load_jsonl(q_log):
        if "ep" not in row:
            continue
        episodes[int(row["ep"])].append(row)

    q_table: Dict[str, Dict[str, float]] = dict(init_model) if init_model else {}
    counts: Dict[str, Dict[str, int]] = defaultdict(dict)
    for s_key, actions in q_table.items():
        for a_key in actions:
            counts[s_key][a_key] = 1  # pseudo-count for loaded entries
    metric_rows = []
    max_ep = 0

    for epoch in range(max(1, epochs)):
        for ep_id in sorted(episodes):
            max_ep = max(max_ep, ep_id)
            steps = episodes[ep_id]
            final_reward = 0.0
            for s in steps:
                if s.get("done") and "final_reward" in s:
                    final_reward = float(s["final_reward"])
                    break

            actions = [s for s in steps if "action_idx" in s]
            total_step_reward = sum(float(s.get("reward", 0.0)) for s in actions)
            returns = final_reward
            for s in reversed(actions):
                reward = float(s.get("reward", 0.0))
                returns = reward + gamma * returns
                skey = state_key(s["state"])
                akey = action_key(s["action_flat"])
                q_entry = q_table.setdefault(skey, {})
                cnt_entry = counts.setdefault(skey, {})
                c = cnt_entry.get(akey, 0)
                old = q_entry.get(akey, 0.0)
                # incremental average to suit offline batch updates
                new_val = (old * c + returns) / (c + 1)
                # lr can optionally smooth toward the new average
                q_entry[akey] = old + lr * (new_val - old)
                cnt_entry[akey] = c + 1

            if epoch == epochs - 1:
                metric_rows.append(
                    {
                        "episode": ep_id,
                        "steps": len(actions),
                        "final_reward": final_reward,
                        "return": returns,
                        "total_step_reward": total_step_reward,
                        "avg_step_reward": (total_step_reward / len(actions)) if actions else 0.0,
                    }
                )

    out_model.parent.mkdir(parents=True, exist_ok=True)
    metrics_csv.parent.mkdir(parents=True, exist_ok=True)

    # versioned snapshot
    if max_ep > 0:
        ep_path = out_model.parent / f"q_table_{datetime.datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        with ep_path.open("w") as f:
            json.dump(q_table, f)

    # latest
    with out_model.open("w") as f:
        json.dump(q_table, f)

    with metrics_csv.open("w", newline="") as f:
        writer = csv.DictWriter(
            f,
            fieldnames=[
                "episode",
                "steps",
                "final_reward",
                "return",
                "total_step_reward",
                "avg_step_reward",
            ],
        )
        writer.writeheader()
        writer.writerows(metric_rows)

    try:
        import matplotlib.pyplot as plt

        episodes_axis = [row["episode"] for row in metric_rows]
        final_rewards = [row["final_reward"] for row in metric_rows]
        avg_rewards = [row.get("avg_step_reward", 0.0) for row in metric_rows]
        plt.figure(figsize=(8, 4))
        plt.plot(episodes_axis, final_rewards, label="final_reward")
        plt.plot(episodes_axis, avg_rewards, label="avg_step_reward")
        plt.xlabel("episode")
        plt.ylabel("reward")
        plt.legend()
        plt.tight_layout()
        metrics_png.parent.mkdir(parents=True, exist_ok=True)
        plt.savefig(metrics_png)
    except Exception:
        pass


def main(argv: List[str]) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--log", type=Path, default=Path("logs/experience.jsonl"))
    parser.add_argument("--out", type=Path, default=Path("models/q_table_latest.json"))
    parser.add_argument("--init-model", type=Path, default=None, help="existing q_table.json to warm-start from (default: --out if present)")
    parser.add_argument("--metrics", type=Path, default=Path("runs/train_metrics.csv"))
    parser.add_argument("--metrics-fig", type=Path, default=Path("runs/train_curve.png"))
    parser.add_argument("--gamma", type=float, default=0.9)
    parser.add_argument("--lr", type=float, default=0.1)
    parser.add_argument("--epochs", type=int, default=2, help="offline epochs over collected episodes")
    args = parser.parse_args(argv)

    if not args.log.exists():
        print(f"log file not found: {args.log}", file=sys.stderr)
        return 1

    init_model_path = args.init_model if args.init_model is not None else args.out
    init_model = load_model(init_model_path)

    train(args.log, init_model, args.out, args.metrics, args.metrics_fig, args.gamma, args.lr, args.epochs)
    print(f"trained model saved to {args.out}")
    print(f"metrics saved to {args.metrics}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

# versions/ — 198 个版本的源码

## 目录结构

```
versions/
  manifest.csv         全量清单：group / slug / zip名 / 来源 / 字节 / SHA256 / 抽取文件数
  INDEX_原始.md        当时的版本索引（含每个版本的一句话说明）
  VisionTrack/         比赛固件（54 版）
  Senior6_RingDiag/    旧架构主线（49 版）
  Senior6_PinAdapt/    引脚适配/早期（61 版）
  RCManual/            遥控固件（6 版）
  CamLine/             摄像头接缝专项（6 版）
  Milestones/          里程碑封印包（22 版）
```

每个版本目录内：

```
<版本名>/
  code/*.c, *.h        算法源码（含当时的 .bak 快照）
  user/*.c, *.h        主程序
  scripts/*.c          当时的测试
  HEX清单.txt          该版本构建出的 HEX 文件名与大小
```

> 说明：**只抽取了源码**（user/ code/ scripts/ 里的 .c/.h + 工程配置），
> 没有放逐飞官方库（`libraries/`）——每个版本 3MB 的库是重复的，
> 需要完整工程时请用对应的原始 zip（见 `manifest.csv` 的 `zip` 列）。

## 怎么用

### 看两个版本差在哪

```bash
cd versions/VisionTrack
diff -ru v145_leftcoop_tighten_91E8FA16_flashed v146_corner_speed_C4E5B2DC_flashed
```

### 找某个参数是什么时候改的

```bash
cd versions/VisionTrack
grep -rn "turncap" --include=*.c . | sort
```

### 找某个 bug 的修复版本

```bash
cd versions
grep -rln "pre_mstamp" .          # v148 的顺序竞态补救
grep -rln "blind_frames" .        # v143 的盲区降速
```

## 命名规则

- `vXXX_<关键词>_<HEX前8位>_flashed.zip` — 已烧录并验证
- `vXXX_<关键词>_unflashed.zip` — 构建但未烧录
- `★MILESTONE_*` / `★GOLDEN_*` / `★★★里程碑_*` — 里程碑封印包
- `vXXX_run_*_capture.zip` — 跑车数据包（不含代码）
- `*_ERR_*` — 已知有问题的包（保留作证据）

## 完整性

`manifest.csv` 里每一行都有原始 zip 的 `sha256`。
需要核对某个包是否被篡改/损坏：

```bash
python - <<'PY'
import csv, hashlib
for r in csv.DictReader(open('manifest.csv', encoding='utf-8-sig')):
    print(r['slug'], hashlib.sha256(open(r['zip'],'rb').read()).hexdigest()[:16])
PY
```

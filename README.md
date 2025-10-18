# directive_mpi

東京大学情報基盤センター お試しアカウント付き講習会「指示文とMPIによるマルチGPUプログラミング入門」で使用するサンプルコード(C, Fortran)です．
指示文（OpenACCあるいはOpenMP target）によるGPU実装とMPIを組み合わせて利用する方法が学べます．
Miyabi-G向けのジョブスクリプトが含まれます．
講習会URL： https://www.cc.u-tokyo.ac.jp/events/lectures/

## Requirement

* NVIDA HPC SDK： https://developer.nvidia.com/nvidia-hpc-sdk-downloads

* GPU-aware MPI
  * Miyabi-G には予めインストールされています

## Usage

以下は全てMiyabi-Gでの利用方法です．

```bash
module load nvidia nv-hpcx   # NVIDA HPC SDK, OpenMPIの環境構築。ログインの度必要です．
cd /work/グループ名/$USER/        #/home は計算ノードから参照できないので、/work以下で作業しましょう．
git clone --recurse-submodules https://github.com/ymiki-repo/directive_mpi.git
cd directive_mpi/
cd C or F                          # C, Fortran好きな方を選んでください。
```

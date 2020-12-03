# waveform-analyzer-c

オシロスコープの生波形を解析するためのツール

## 依存関係

* CERN ROOT

## ビルド

```
cd src && make 
```

これで、readRawFile と makeHistogram が出来あがる。

## 使い方

このツールは大きく2つのプログラムからなる:
- readRawFile: 生波形に対して、一定の閾値を掛けて、越えた時刻を求め、トリガ時刻からのクロックエッジまでの時間、または各クロック1周期の周期を求め出力する。
- makeHistogram: 標準入力からの値からヒストグラムを描く。
2つを繋げることで、生波形からの周期ジッターのプロットなどを描くことができる。

簡単な例:
```
./readRawFile < waveform.csv | ./makeHistogram
```
これを実行すると、"out.root" というファイルが生成され、このファイルには TH1F のヒストグラムが1つ保存される。
以下で、詳細なオプションを説明する。

### readRawFile

```
./readRawFile {[raw-csv_file]} {options}

Usage:
[raw-csv_file]: waveform-file of oscilloscope (should be csv format of Tektronix for now)

OPTIONS: -h, --help, --threshold, --mestype, --trigger, --period, --autothreshold
	-h, --help: show this help
	--mestype {trigger, period}: select measurement type: trigger=relative timing, period=calculate period
	--trigger: equivalent to "--mestype trigger"
	--period: equiavalent to "--mestype period" (default)
	--autothreshold: set threshold to 50% of amplitude
```

生波形は基本的には標準入力から読み込ませれば十分。標準入力の他、ファイル名を指定しても同じように解析ができる。

解析では、2つの測定モード (period, trigger) を実装した。"period"モードは、周期を計算するモード。"trigger"モードは、オシロがトリガーを掛けた時刻を読み、トリガー時刻以降でもっとも近いクロックエッジを探す。そして、クロックエッジとトリガー時刻の差を計算する。
"threshold" オプションで閾値を自由に決定できる (default では 1.0 [V])。"autothreshold"オプションでは自動的に50%の閾値を計算して、これを使ってエッジを求める。

閾値との交点は、現在のところ線形補間で求めているだけとなっている。

### makeHistogram

```
./makeHistogram {options}

Usage:
OPTIONS: -h, --help, -o, --xmin, --xmax, --nbins
	-h, --help: show this help
	-o {filename}: write TH1 histogram into this file (in default, the histogram is saved in "out.root")
	--xmin {min}: left edge of TH1F
	--xmax {max}: right edge of TH1F
	--nbins {nbin}: number of bins of TH1F
```

ヒストグラムの上限下限、ビン数を設定できる。
"-o"オプションで出力ファイルの名前を設定する (default では "out.root")。

### 参考例

ディレクトリ data/ に複数の波形データ(xxx.csv) がある場合:

```
analyze.sh
--------------------
for f in data/*.csv
do
    readRawFile --threshold 1.24 --mestype period $f
done
```

の様に analyze.sh という shellスクリプトを用意した上で、

```
./analzze.sh | ./makeHistogram --xmin 1e-9 --xmax 9e-9 --nbins 500 -o all.root
```

とすれば、全データファイル分の周期が計算されてヒストグラムが"all.root"というファイルに出力される。



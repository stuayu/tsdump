# tsdump
- [tsdump](#tsdump)
	- [このツールについて](#このツールについて)
	- [動作環境](#動作環境)
		- [Windows](#windows)
		- [Linux](#linux)
		- [それ以外](#それ以外)
	- [主要なコマンドオプション](#主要なコマンドオプション)
		- [Windows](#windows-1)
		- [Linux](#linux-1)
		- [共通](#共通)
	- [コマンド例](#コマンド例)
		- [Windows](#windows-2)
		- [Linux](#linux-2)
	- [ドキュメント](#ドキュメント)
	- [ライセンス](#ライセンス)
	- [連絡先](#連絡先)

## このツールについて

概要: いわゆる「全録」をPCで行うための軽量で安定したコマンドライン録画ツール

引数で指定したチューナー、チャンネルの番組をひたすら指定したディレクトリに書き出し続けるのがメインの機能です。\
その他、以下の特徴があります。

- 主にC言語で書かれていて高速で安定しているので月オーダーの連続動作にも耐える
- Windows(BonDriver)とLinux(DVB)に対応
- 番組情報対応（番組自動分割）
- 番組の前後のマージン録画対応
- TSを外部プログラムにパイプで渡す機能があり様々な外部プログラムと連携可能
- モジュール用のAPIがあり内部構造に直結した機能拡張も可能


## 動作環境

### Windows
VS2017で動作テストをしています(プロジェクトファイルそのものは2013で作ったものを使いまわしているのでバージョン12になっています)。\
XP以降のWindowsならなんでも動くかとは思いますが、Windows10で動作確認をしています。\
チューナーAPIはBonDriver入力モジュールを標準搭載しています。

### Linux
**libarib25**(→stz2012/libarib25)と**DVBインタフェース**(一般的なLinux環境には存在)が必要です。\
\
Ubuntu16.04とDebian8.xでmakeが通ることを確認しています。\
多少のMakefileなどの修正は必要かもしれませんが最近のLinux(+gcc)ならビルド可能かとは思います。\
CP932のファイルを入力できないのでclangではビルドできません、今のところgccのみでビルドできます。\
MULTI2デコード機能が不要であればmod_arib25関連の記述を削除することでlibarib25に依存せずビルドできます。\
チューナーAPIはDVB入力モジュールを標準搭載しています。

DVBはチューナーごとに機種依存な挙動があり、多少の修正をしないと正常に動作しないチューナーがあるかもしれません。\
特に信号強度やCNRの取得方法や単位はチューナーやドライバによってバラバラです。

一応Ubuntu16.04(Kernel4.4)における以下のチューナーの動作は確認しています
- friio(白) 地上波
- PT1/2 地上波、BS/CS
- PT3 地上波、BS/CS　←このチューナーのドライバは新しいDVBのインタフェースに則っており最も標準性が高い
- PX-S1UD V2.0 地上波
- TBS-6814 地上波 (TBS6814 Linux Driver v160630) ※メーカー提供のプロプライエタリドライバ

### それ以外
プラットホーム向けのチューナーAPIとデコードライブラリが上記2プラットホームのものしか組み込まれていないので\
スタンドアローンの録画ツールとしては意味をなさないですが、\
gccの存在するUNIXライクOSならば多少の修正でビルド自体は可能かと思います。\
たとえば、Cygwinでファイル入力を使って開発中のLinux対応部分の動作テストをしています。

---
## 主要なコマンドオプション

### Windows
```pwsh
--bon [BonDriver.dll] *
--sp [チューナー空間] *
	TVTest等参照
--ch [チャンネル] *
--b25dec B25Decoder.dllを用いてMULTI2をデコードする
```

### Linux
```pwsh
--dev [DVBのデバイス番号]
	指定しなければ空いている使用可能なデバイスが自動的に選ばれる
--type [DVBのチューナータイプ] *
	isdb-tあるいはbsあるいはcs
--ch [物理チャンネル] *
	物理チャンネル番号
--tsnum [TS番号]
	BSのときのみ指定する
--arib25 libarib25を用いてMULTI2をデコードする
```

### 共通
```pwsh
--dir [出力ディレクトリ] *
	TVTest等参照
--sv [サービス](複数指定可)
	例: 
	--sv 333 --sv 334　（複数指定の例）
	--sv all
	など
--filein [入力ファイル]
	tsファイルをファイルから入力して仮想的なチューナーとして用いる機能で、主にテスト用
--no-fileout ファイルを出力しない
	パイプのみに出力したい場合など
--cmd [終了時実行コマンド](複数指定可)
	ファイルクローズの際に呼ばれる
--cwmin 終了時実行コマンドを最小化状態で起動する
--pipecmd [パイプ実行コマンド](複数指定可)
	ファイルに書き出すのと同じ内容がパイプで送り込まれる
--pipeconn パイプ実行コマンドの出力を次のコマンドの入力に接続する
--setopt [実行コマンドのオプション](複数指定可)
--nopipe 実行のみでパイプ出力を行わない
--cmd-output-redirect [dir]
	各コマンドの標準出力／エラー出力を指定したディレクトリに書き出す
--pwmin パイプ実行コマンドを最小化状態で起動する
--log ログを出力する
--logfile [ログのファイル名] ログのファイル名を指定する
	指定しない場合は、カレントディレクトリの"プロセスID.log"にログが出力される
```

\* : 必須オプション

※組み込むモジュールによってオプションは増減します
※コマンドオプションなしで実行すると使用可能な全オプションが表示されます


## コマンド例

### Windows
```pwsh
tsdump --bon BonDriver_Hoge.dll --sp 0 --ch 14 --dir D:\
```
**スペース0**（通常、地上波:0 BS:0 CS:1）\
BonDriver**チャンネル14**（通常、物理チャンネル27ch、関東だとNHK総合・東京）\
をD:\に録画する\
※スペースやBonDriverチャンネルはBonDriverの設定ファイルやTVTestなどで確認可能

### Linux
```pwsh
tsdump --type isdb-t --ch 27 --dir /data/rec
```

/data/recに27chを録画する

> [!TIP] 
> 発展的な使い方\
	`tsdump (略: 録画オプション、要MULTI2デコード) --pipecmd ffmpeg --setopt "-i - -vcodec libx264 -acodec aac -preset ultrafast \"{FILENE}.mp4\""`\
	ffmpegを用いて録画と同時にエンコードして同じディレクトリに.mp4ファイルを出力する\
	--no-fileoutを付ければ.mp4のみ保存する


## ドキュメント
すみません、現状このreadme以上のドキュメントはありません。
詳しくはソースコードを読んでください。


## ライセンス
**使用は各々の責任で。**

現状aribstr.cがGPLのコードを元にしているので全体としては**GPL**になっています。\
aribstr.cと関わらない部分であれば使用者の責任のもとで煮るなり焼くなり好きにどうぞ。


## 連絡先
Twitter: @homuh0mu

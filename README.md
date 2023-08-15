# M5JapaneseInput

M5StackにCardKBを接続してローマ字ひらがな入力・単語または単漢字変換をしたかったPlatformIOプロジェクトです。

## 必要品

- M5Stack (Basicを使用しているためピン設定は変える必要があるかもしれません)
- CardKB
- microSDカード（辞書ファイルを入れるのに使用します）
- SKK辞書ファイル（SKK-JISYO.MかSをUTF-8に変換して使用します）

## ビルドと起動

- PlatformIOプロジェクトを読み込みます。
- microSDカード直下にkanadic.txt、およびUTF-8に変換したSKK-JISYO.MかSを配置します。
- main.cppでSKK-JISYO.MかSを指定し、ビルドします。

## キーボードの操作

- 再度電源を入れた時は最後のnoteX.txtから内容を読み込みます。
- CardKBからローマ字入力するとひらがなに変換されます。
- ひらがなに変換された状態でSpaceキーを押すと漢字変換候補を表示します。
- Enterキーで確定し、文字列を追加します。
- ローマ字やひらがなを入力していない状態でSpaceやEnterキーを押すと空白、改行になります（カーソルがないので位置が見えないですが）
- CardKBでSymを押すと記号入力になりますが、この状態でG（-）を押すと伸ばし棒（ー）に変換します。
- ESCを押すとdeep sleepします。

## ボタンの機能

- 左ボタン（BtnA）を押すと現在の内容をnoteX.txtに保存し、deep sleepします。
- 中央ボタン（BtnB）を押すと現在の内容を消去します。
- 右ボタン（BtnC）を押すと現在の内容を新規のnoteX.txt（Xは重複のない数字）に保存します。
# M5JapaneseInput

M5StackにCardKBを接続してローマ字ひらがな入力・単語または単漢字変換をしたかったPlatformIOプロジェクトです。

## 必要品

- M5Stack (Basicを使用しているためピン設定は変える必要があるかもしれません)
- CardKB
- microSDカード（辞書ファイルを入れるのに使用します）
- SKK辞書ファイル（SKK-JISYO.MかSをUTF-8に変換して使用します）

## 使い方

- PlatformIOプロジェクトを読み込みます。
- microSDカード直下にkanadic.txt、およびUTF-8に変換したSKK-JISYO.MかSを配置します。
- main.cppでSKK-JISYO.MかSを指定し、ビルドします。
- 左ボタン（BtnA）を押すと現在の内容をnoteX.txtに保存し、deep sleepします。
- 中央ボタン（BtnB）を押すと現在の内容を消去します。
- 右ボタン（BtnC）を押すと現在の内容を新規のnoteX.txt（Xは重複のない数字）に保存します。
- 再度電源を入れた時は最後のnoteX.txtから内容を読み込みます。
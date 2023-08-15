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

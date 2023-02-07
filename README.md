# PokeControllerForPico

[Poke Controller Modified](https://github.com/Moi-poke/Poke-Controller-Modified)とSwitch間を中継するソフトウェア

# このソフトウェアについて

ろっこく氏のPoke-ControllerForLeonardoを参考にして作っています

# 導入方法

必要なもの

    Raspberry Pi Pico 1台
    USB-UART変換 1台
    USB ロケーブル 2本(PCとUSB-UART変換、Raspberry Pi PicoとSwitchの接続用)

動作環境

    Arduino IDE(1.8.19推奨)
    Arduino-Pico(v2.7.1)

# 使い方

## ソフトウェアのダウンロード

このリポジトリをダウンロードし適当なフォルダへ展開

## 書き込み

1. Raspberry Pi PicoのBOOT SELボタンを押しながらPCと接続
2. RPI-RP2デバイスが認識される。フォルダは閉じる
3. Arduino IDEを立ち上げる
4. ツール→ボード→Raspberry Pi RP2040 Board(2.7.1)→Raspberry Pi Picoを選択
5. ツール→USB Stack→No USBを選択
6. ファイル→開くでスケッチ(PokeControllerForPico.ino)を開く
7. スケッチ→マイコンボードへ書き込む
8. 書き込みが終わったら、Raspberry Pi PicoとSwitchを接続
9. Raspberry Pi PicoとUSB-UART変換を接続する

~~~
Raspberry Pi Pico 1番ピン(UART0TX) <-------> USB-UART変換 RX
Raspberry Pi Pico 2番ピン(UART0RX) <-------> USB-UART変換 TX
Raspberry Pi Pico 3番ピン(GND)　　 <-------> USB-UART変換 GND
~~~

10. USB-UART変換をPCと接続

Poke Controller側のCOM Port番号をUSB-UART変換のものに合わせてSwitch Controller Simulatorなどでボタンを押して動作確認してください

# Lisence

このプロジェクトのライセンスはMITライセンスです。詳細はLICENSEをご覧ください
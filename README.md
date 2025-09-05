# PokeControllerForPico

[Poke Controller Modified](https://github.com/Moi-poke/Poke-Controller-Modified)や[Poke Controller Modified Extension](https://github.com/futo030/Poke-Controller-Modified-Extension)とSwitch間を中継するソフトウェア

# このソフトウェアについて

ろっこく氏のPoke-ControllerForLeonardoを参考にして作っています

# 導入方法

必要なもの

- [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/)又は[Raspberry Pi Pico2](https://www.raspberrypi.com/products/raspberry-pi-pico-2/) 1台
- USB-Serial変換器 1台
- USB ケーブル 2本(PCとUSB-Serial変換器、Raspberry Pi PicoとSwitchの接続用)
- PicoとUSB-Serial変換器を接続用のジャンパ線等
    - Raspberry Pi Pico Hなどの名前で売られている半田付け済のモデルを購入しない場合、自前で半田付けする必要がある

# 使い方

## ソフトウェアのダウンロード

このリポジトリの[Releasesページ](https://github.com/yqYo1/PokeControllerForPico/releases)からボードに対応するuf2ファイルをダウンロードする
- Picoの場合は`PokeControllerForPico-pico.uf2`を、Pico2の場合は`PokeControllerForPico-pico2.uf2`のファイルをダウンロードする

## 書き込み

1. PicoのBOOT SELボタンを押しながらPCと接続
2. Picoがストレージとして認識されるのでファイラーで開く
3. ダウンロードしたuf2ファイルをフォルダにコピーする
4. 書き込みが終わったら、Raspberry Pi PicoとSwitchを接続
5. Raspberry Pi PicoとUSB-Serial変換器を接続する

~~~
Raspberry Pi Pico 1番ピン(UART0TX) <-------> USB-Serial変換器 RX
Raspberry Pi Pico 2番ピン(UART0RX) <-------> USB-Serial変換器 TX
Raspberry Pi Pico 3番ピン(GND)     <-------> USB-Serial変換器 GND
~~~

6. USB-Serial変換器をPCと接続

Poke Controller側のCOM Port番号をUSB-Serial変換器のものに合わせてSwitch Controller Simulatorなどでボタンを押して動作確認してください

# Lisence

このプロジェクトのライセンスはMITライセンスです。詳細は[LICENSE](https://github.com/yqYo1/PokeControllerForPico/blob/master/LICENSE)をご覧ください

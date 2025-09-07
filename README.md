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

# 開発者向け(Building from Source)

このプロジェクトを自身でビルドしたい開発者向けのガイドです。

## 必要なもの (Prerequisites)

- Git
- CMake
- GNU Arm Embedded Toolchain (`gcc-arm-none-eabi`)
- Make

DebianベースのOS(Ubuntuなど)では、以下のコマンドでインストールできます。
```bash
sudo apt-get update && sudo apt-get install -y git cmake make gcc-arm-none-eabi
```

## ビルド手順 (Build Steps)

1. **このリポジトリをクローンします。**
   ```bash
   git clone https://github.com/yqYo1/PokeControllerForPico.git
   cd PokeControllerForPico
   ```

2. **Pico SDKをクローンします。**
   このプロジェクトは特定のバージョンのPico SDKに依存しています。CIではバージョン `2.2.0` を使用しています。
   ```bash
   # プロジェクトと同じ階層にSDKをクローンする場合
   git clone --branch 2.2.0 --depth 1 https://github.com/raspberrypi/pico-sdk.git ../pico-sdk
   ```

3. **Pico SDKのサブモジュールを初期化します。**
   ```bash
   cd ../pico-sdk
   git submodule update --init
   cd ../PokeControllerForPico
   ```

4. **環境変数を設定します。**
   ビルドシステムがPico SDKを見つけられるように、`PICO_SDK_PATH`環境変数を設定する必要があります。
   ```bash
   export PICO_SDK_PATH=/path/to/your/pico-sdk
   ```
   *注意: `/path/to/your/pico-sdk` は、ステップ2でクローンしたpico-sdkディレクトリへの絶対パスに置き換えてください。*

5. **ファームウェアをビルドします。**
   リポジトリのルートで`make`コマンドを実行します。
   ```bash
   # Raspberry Pi Pico向けにビルド
   make pico

   # Raspberry Pi Pico 2向けにビルド
   make pico2
   ```
   ビルドが成功すると、リポジトリのルートに `.uf2` ファイル (例: `PokeControllerForPico-pico.uf2`) が生成されます。

# Lisence

このプロジェクトのライセンスはMITライセンスです。詳細は[LICENSE](https://github.com/yqYo1/PokeControllerForPico/blob/master/LICENSE)をご覧ください

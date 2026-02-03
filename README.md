# PokeControllerForPico

[Poke Controller Modified](https://github.com/Moi-poke/Poke-Controller-Modified)や[Poke Controller Modified Extension](https://github.com/futo030/Poke-Controller-Modified-Extension)とSwitch間を中継するソフトウェア

# このソフトウェアについて

ろっこく氏のPoke-ControllerForLeonardoを参考にして作っています

# 導入方法

必要なもの

- [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/)、[Raspberry Pi Pico2](https://www.raspberrypi.com/products/raspberry-pi-pico-2/)又は[Waveshare RP2040-Zero](https://www.waveshare.com/rp2040-zero.htm) 1台
- USB-Serial変換器 1台
- USB ケーブル 2本(PCとUSB-Serial変換器、Raspberry Pi PicoとSwitchの接続用)
- PicoとUSB-Serial変換器を接続用のジャンパ線等
    - Raspberry Pi Pico Hなどの名前で売られている半田付け済のモデルを購入しない場合、自前で半田付けする必要がある

# 使い方 (How to Use)

## 1. ハードウェアの準備 (Hardware Setup)

**接続図 (Wiring Diagram):**

| Raspberry Pi Pico |      | USB-Serial変換器 (USB-Serial Converter) |
| :---------------- | :--- | :------------------------------------- |
| 1番ピン (GP0/TX)  | ↔    | RX                                     |
| 2番ピン (GP1/RX)  | ↔    | TX                                     |
| 3番ピン (GND)     | ↔    | GND                                    |

上記のように、Raspberry Pi PicoとUSB-Serial変換器をジャンパー線で接続します。

## 2. ファームウェアの書き込み (Flashing the Firmware)

1.  **ファームウェアをダウンロードします。**
    このリポジトリの[Releasesページ](https://github.com/yqYo1/PokeControllerForPico/releases)から、お使いのボードに対応する `.uf2` ファイルをダウンロードします。
    -   Pico: `PokeControllerForPico-pico.uf2`
    -   Pico2: `PokeControllerForPico-pico2.uf2`
    -   Waveshare RP2040-Zero: `PokeControllerForPico-waveshare_rp2040_zero.uf2`

2.  **PicoをBOOTSELモードでPCに接続します。**
    Picoの「BOOTSEL」ボタンを押しながら、PCにUSBケーブルで接続します。

3.  **ファームウェアをコピーします。**
    Picoが「RPI-RP2」という名前のUSBストレージとしてPCに認識されます。ダウンロードした `.uf2` ファイルを、そのストレージにドラッグ＆ドロップでコピーします。コピーが完了すると、Picoは自動的に再起動します。

## 3. 接続と動作確認 (Connection and Verification)

1.  書き込みが終わったら、Raspberry Pi PicoをNintendo Switchに接続します。
2.  USB-Serial変換器をPCに接続します。
3.  Poke Controller側のシリアルポート設定を、PCに接続したUSB-Serial変換器のCOMポート番号に合わせます。
4.  Switch Controller Simulatorなどでボタンを押し、Switch上でコントローラーとして認識・動作することを確認してください。

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
   CI(継続的インテグレーション)ではPico SDKのバージョン `2.2.0` を使用してビルドしています。異なるバージョンでも動作する可能性がありますが、予期せぬ問題を防ぐため、同じバージョンを使用することを推奨します。
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

   # Waveshare RP2040-Zero向けにビルド
   make waveshare_rp2040_zero
   ```
   ビルドが成功すると、`build/` ディレクトリ内に `.uf2` ファイル (例: `PokeControllerForPico-pico.uf2`) が生成されます。

# Lisence

このプロジェクトのライセンスはMITライセンスです。詳細は[LICENSE](https://github.com/yqYo1/PokeControllerForPico/blob/master/LICENSE)をご覧ください

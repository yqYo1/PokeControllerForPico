# MCUコマンド (MCU Commands)

このソフトウェアは、外部ソフトウェア（PC側）から特定の文字列（コマンド）を受け取ると、MCU単独でNintendo Switchへ決められた順序・時間間隔でボタン入力を自動送信する組み込みマクロ機能を持っています。これにより、PC側の処理負荷や通信ラグに依存しない安定した自動化が可能になります。

また、C++コードを編集することで、ユーザー独自のマクロコマンドを定義することも想定されています。

## 1. 組み込みマクロ一覧

PCから以下の文字列（末尾に改行 `\n`）を送信すると実行されます。一部は乱数消費や特定ゲームの定型作業向けに特化しています。
基本的にろっこくさんのPoke-ControllerForLeonardoに定義されているMCUコマンドに準拠した動作になっている為日付変更等一部のMCUコマンドは現在のswitchではうまく動作しません。

- **`mash_a`**
  - **内容:** Aボタンのみを連続して押す（A連打）。
- **`aaabb`**
  - **内容:** Aを3回、その後Bを2回押す。
- **`auto_league`**
  - **内容:** A連打を基本としつつ、稀にBを押すなどのトーナメント戦用反復行動。
- **`inf_watt`**
  - **内容:** HOME画面から設定に移動し、日付関連の操作を含む複雑な反復行動を行う（W（ワット）回収用）。
- **`pickupberry`**
  - **内容:** きのみの回収を自動化するマクロ。
- **`Date <Year>/<Month>/<Day>`** (*※引数指定型*)
  - **内容:** 指定した年/月/日になるように時間を進める。
- **`Year <Year>`** (*※引数指定型*)
  - **内容:** 指定した年数分だけ大きく時間を進めるためのマクロ。

## 2. その他のMCUシステムコマンド

マクロとは異なり、システム（Pico）の状態を操作するための特殊コマンドです。

- **`p_sync`**
  - **内容:** （機能拡張向けの予約）同期状態フラグを変更します。
- **`p_unsync`**
  - **内容:** （機能拡張向けの予約）同期解除状態としてフラグを変更します。
- **`end`**
  - **内容:** 実行中の組み込みマクロを全て強制終了し、スティックとボタンの入力をニュートラルな状態（初期状態）にリセットします (`ResetDirections()`)。

---

## 3. 独自のMCUコマンドを追加する方法（開発者向け）

もし、独自の新しいマクロをPicoに自己完結で実行させたい場合は、以下の手順でソースコードを変更して再ビルドしてください。

### 手順1: `PokeControllerForPico_Func.h` にコマンドを定義する

新しいマクロのステップは `SetCommand` 型の配列として定義します。この構造体は以下の3つの要素を持っています。

```cpp
typedef struct {
  uint8_t command;  // 押下または操作するボタン/スティック (例: COMMAND_A)
  int duration;     // 押下している維持時間（ミリ秒）
  int waittime;     // その後、入力を離してから待機する時間（ミリ秒）
} SetCommand;
```

例として、「Bボタンを短く押し、1秒待ってAボタンを長く押す」という配列を `PokeControllerForPico_Func.h` に追加します。

```cpp
const SetCommand my_custom_macro[] =
{
  {COMMAND_B, 50, 1000},  // Bを50ms押し、1000ms待機
  {COMMAND_A, 500, 200}   // Aを500ms押し、200ms待機
};

// 配列のサイズも定義しておく必要があります
const int my_custom_macro_size = (int)(sizeof(my_custom_macro) / sizeof(SetCommand));
```

利用可能なコマンド名（例：`COMMAND_HOME`, `COMMAND_UP` など）は、同ファイルの `BUTTON_DEFINE` 列挙型に定義されています。

### 手順2: 状態を定義してコマンドと状態を紐づける

次に、`PokeControllerForPico_Func.cpp` を開きます。

コマンドの状態列挙型 `Proc_State_t` に新しい状態を追加します。
```cpp
typedef enum {
  NONE,
  // ...既存の定義...
  MY_CUSTOM_MACRO, // ← これを追加
  // ...
};
```

受信する文字列（PCから送信する文字列名）を `cmd_name` の配列に追加し、`ParseLine` 関数内で分岐処理を追加します。

```cpp
// cmd_name配列の最後に足すか、独立してstrcmpで判定させても良いです
else if (strncmp(cmd, "my_custom_macro", 15) == 0) {
  proc_state = MY_CUSTOM_MACRO;
  ProgState = STATE1;
}
```

### 手順3: 実行分岐 (`SwitchFunction()`) に処理を追加する

同じく `PokeControllerForPico_Func.cpp` 内にある `SwitchFunction()` 関数に、追加したコマンドが実行された際の挙動を追記します。

```cpp
void SwitchFunction(void)
{
  switch (proc_state)
  {
    // ...
    case MY_CUSTOM_MACRO:
      // 配列の先頭ポインタとサイズを渡すだけで自動的にループ処理が実行されます
      GetNextReportFromCommands(&my_custom_macro[0], my_custom_macro_size);
      break;
    // ...
  }
}
```

これで、PCから `"my_custom_macro\n"` という文字列を送信するだけで、Picoが自律的に指定したボタン操作シーケンスを実行するようになります。

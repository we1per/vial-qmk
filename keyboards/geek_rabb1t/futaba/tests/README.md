# Futaba 統合検証

リポジトリのルートから、Pythonとホスト用GCCが利用できる環境で実行する。

```sh
python keyboards/geek_rabb1t/futaba/tests/integration_test.py
make geek_rabb1t/futaba:vial
make geek_rabb1t/futaba:default
```

重点テストは実際のSmart Alt+Tabヘッダー、quadratureドライバー、共有trackpad設定コードをコンパイルする。長押し、連打、同時押し、重複イベント、操作前から押されていたキーのrelease、通常Left Altとの混在、レイヤー離脱、16bitタイマー周回、encoderとgestureのイベントを確認する。encoderではresolution 4とoffset 1の早期発火、連続回転、方向転換、無効遷移、複数encoderの独立性、offset未定義・0・方向反転・default position・個別resolutionを確認する。上流ドライバーにもoffset未定義時の同じテストを実行する。

共有trackpad設定はdynamic keymapから読み、速度・補正・スクロール・慣性・回転設定を確認する。上流ソースが非dynamic構成でコンパイルに失敗することも再現する。Futabaのdefault keymapは上流で共有実装へ未移植であり、旧driver include・旧LAYOUT・旧設定構造が残っている。defaultビルドは実行したが失敗し、検証成功には含めない。今回のVial統合では旧実装を復元せず、defaultの移植は別の変更として扱う。

既存QMK encoderテスト7構成も実行する。WindowsのMinGW GCC/COFFリンカーでweak関数が未解決になる場合、実関数へのリンクaliasだけを追加する。テストや関数本体は変更しない。single構成では以下を指定し、split構成では`encoder_driver_task`のaliasだけを指定する。

```sh
make -r -R -f builddefs/build_test.mk TEST=encoder TEST_OUTPUT=encoder TEST_PATH=encoder \
  BUILD_DIR=.build/integration-existing-tests SILENT=false \
  EXTRALDFLAGS="-Wl,--defsym,encoder_driver_task=.weak.encoder_driver_task.encoder_quadrature_post_init -Wl,--defsym,is_keyboard_master=.weak.is_keyboard_master.mock_set_pin_input_high"
.build/integration-existing-tests/test/encoder.elf
```

`ENCODER_EVENT_OFFSET`は0以上かつ各encoderのresolution未満とし、`ENCODER_DEFAULT_POS`とは併用しない。未定義のキーボードは上流と同じ処理を使う。

## 実機確認と設定移行

旧we1per版のmatrixは11×6、新しい上流版は16×9で、EEPROM内のencoder・macros・追加設定の配置も変わる。`ALT_TAB`は旧`0x7E06`から`0x7E20`へ移動した。UIDは同一でも旧EEPROMや旧Vial保存ファイルをそのまま復元できるとは扱わない。VIAのbuild IDが変わるため初回起動時に設定が初期化される可能性もある。書き込み前にユーザー側で旧設定を保存し、新定義で物理キー・encoder・macros・追加設定を再構成する。

新しいVial定義で`ALT_TAB`を割り当て、layer 7の操作キーを設定する。未設定のdynamic layerは透過キーなのでlayer 7へ入れるが、独自の操作配置は自動移行されない。旧スクロール反転・3finger設定は新しい上流の設定行・tap設定へ対応を確認して設定する。

最小限の実機確認は、通常入力、Alt+Tabの連打/長押し/800ms後の解放/レイヤー離脱/通常Alt混在、encoderの左右回転と方向転換、trackpadの移動/クリック/gesture/高解像度スクロール/慣性、Vialの8レイヤーと保存・再接続。ホストテストはUSBレポートの配送、実パルス、実trackpadやOSの動作を検証しない。

通常Left Altとの混在テストは単独の`KC_LALT`割当てを対象とする。複数位置への同一modifier割当てやmod-tap/one-shotによるAltの組合せは実機確認対象に残る。

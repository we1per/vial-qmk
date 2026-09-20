# Futaba 統合後の実機確認手順

統合版を実機確認し、[PR #3](https://github.com/we1per/vial-qmk/pull/3)をレビュー可能な状態へ進めるための手順書。以下を上から順番に実施し、問題が出たら次へ進まない。

## 現在の状態

- ファームウェア: `.build/integration-final-vial/geek_rabb1t_futaba_vial.uf2`
- SHA-256: `f82aed6fbf2d7084a7180f9b0d00e569723f136a9ece7556b16eeab3077047d2`
- ソフトウェア検証: 完了
- 実機確認: 未実施
- GitHub Actions: 手動無効のため未実施
- Futaba default keymap: 上流の未移植部分があり、ビルド非対応

## 実施する5工程

1. 現在のVial設定を記録する
2. UF2を照合して書き込む
3. 新しい定義でVial設定を作り直す
4. 実機動作を確認する
5. 結果をPRへ記録する

## 1. 現在のVial設定を記録する

書き込み前に、次をスクリーンショットまたはメモで保存する。

- layer 0〜7のキー配置
- encoderの左右回転への割り当て
- macro、tap dance、comboなどの追加設定
- trackpadの速度、加速度、スクロール、慣性、反転、tap設定
- `ALT_TAB`を割り当てているキーとlayer 7の配置

**完了条件:** 上記を見ながら手動で再設定できる。

旧設定ファイルは参考資料として保存するが、新ファームウェアへ一括復元しない。matrixが11×6から16×9へ変わり、`ALT_TAB`も`0x7E06`から`0x7E20`へ移動しているため、旧データをそのまま適用できない。

## 2. UF2を照合して書き込む

PowerShellでハッシュを確認する。

```powershell
Get-FileHash .build\integration-final-vial\geek_rabb1t_futaba_vial.uf2 -Algorithm SHA256
```

結果が次と一致した場合だけ書き込む。

```text
F82AED6FBF2D7084A7180F9B0D00E569723F136A9ECE7556B16EEAB3077047D2
```

1. Vialを閉じる
2. Futabaをブートローダーモードにする
   - Escを押したままUSB接続する
   - 反応しない場合は基板のresetボタンを使う
   - `QK_BOOT`を割り当て済みなら、そのキーも利用できる
3. 表示されたRP2040ドライブへUF2をコピーする
4. 自動再起動後、通常のキーボードとして認識されることを確認する

**完了条件:** OSがFutabaを認識し、意図しない連続入力がない。認識しない場合は設定作業へ進まない。

## 3. Vial設定を作り直す

新ファームウェアをVialで開き、次の順に手動設定する。

1. layer 0の通常キー
2. 使用しているlayer 1〜6
3. `ALT_TAB`を割り当てる物理キー
4. Smart Alt+Tab中に使うlayer 7のキー
5. encoderの左右回転
6. macro、tap dance、combo
7. trackpadの速度、加速度、スクロール、慣性、反転、tap

設定後にUSBを抜き差しして、Vial上の設定が残っていることを確認する。

**完了条件:** 必要な設定を新しい定義で再作成し、再接続後も保持される。

## 4. 実機動作を確認する

次の順で確認する。前の項目がNGなら、後続項目へ進まない。

| 項目 | 確認内容 |
| --- | --- |
| 通常入力 | 全物理キー、layer 0〜6、Shift/Ctrl/Alt/GUIの解除 |
| Smart Alt+Tab | 1回押し、連打、長押し、layer 7操作、最後のreleaseから800ms後のAlt解除、途中のlayer離脱、通常Left Altとの併用 |
| Encoder | 左右1ノッチ1入力、連続回転、回転方向をすぐ反転した際の取りこぼし・二重入力がないこと |
| Trackpad | 移動、クリック、gesture、縦横スクロール、反転、高解像度スクロール、慣性 |
| 保存 | USB再接続後もキー、encoder、macro、trackpad設定が残り、layer 7を再編集できること |

Smart Alt+Tab確認後は通常文字を入力する。メニューが反応し続けるなどAlt保持の兆候があればNGとする。

**完了条件:** 表の全項目がOK。

## 5. 結果をPRへ記録する

PR #3へ次の形式で記録する。

```text
実機確認日:
OS:
使用したUF2のSHA-256:

- 通常入力: OK / NG
- Smart Alt+Tab: OK / NG
- Encoder: OK / NG
- Trackpad: OK / NG
- 設定保存・再接続: OK / NG

NGの再現手順・補足:
```

- 全項目OK: PRをDraftからレビュー可能な状態へ変更できる
- 1件でもNG: Draftのままにしてマージしない

## 問題が出た場合

EEPROM初期化や設定の全消去を先に試さない。次を記録して、問題のある機能の使用を止める。

- 症状と発生頻度
- 最短の再現手順
- 使用したlayerとキー配置
- Vialの該当画面
- OSとVialのバージョン
- 使用したUF2のSHA-256
- USB再接続後も再現するか

以前動作していたUF2を保存している場合だけ、そのファイルへ戻して復旧する。戻す場合も、旧Vial設定を新ファームウェアへ一括復元しない。

## 開発者向け再検証

```sh
python keyboards/geek_rabb1t/futaba/tests/integration_test.py
make geek_rabb1t/futaba:vial
```

Futabaのdefault keymapは上流で共有trackpad実装への移植が完了していない。次のビルドは現在失敗するため、Vial検証の成功と混同しない。

```sh
make geek_rabb1t/futaba:default
```

Windowsで既存QMK encoderテストを実行する際は、MinGW GCC/COFFのweak関数解決にリンクaliasが必要な場合がある。詳細な実行条件はPR #3の検証記録を参照する。

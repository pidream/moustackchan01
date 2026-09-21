# ハードウェアの概要
  https://www.pidream.net/2024/04/240430.html
 
# 開発環境

## Arduino IDE
- 使用バージョン：**Arduino IDE 2.3.6**  

## ボードマネージャー
- 3.0系のボード設定ファイルに対応しました。  
https://docs.espressif.com/projects/arduino-esp32/en/latest/migration_guides/2.x_to_3.0.html  
https://docs.espressif.com/projects/arduino-esp32/en/latest/api/timer.html  

## ライブラリ
- 使用するライブラリは以下の通りです。  
  ![ライブラリ1](https://github.com/user-attachments/assets/76f8bc22-9d23-48ed-ae4e-f520a0381480)  
  ![ライブラリ2](https://github.com/user-attachments/assets/8f553a63-e84e-4b89-bba9-ffe1f9b6c8f8)  
  ![ライブラリ3](https://github.com/user-attachments/assets/ee71896c-1572-43e6-b544-673ca643a74f)  

---

# フォルダ構成

すべてのフォルダは **`moustackchan01`** フォルダの下にまとめて配置してください。  


---

# 走行モード（run_mode）

起動すると待機ループに入り、`run_mode` を 0〜5 から選んで実行します。

## ボタン操作

| ボタン | 動作 |
| --- | --- |
| A（または電源ボタン 短押し） | `run_mode` を −1（0 の次は 5 に戻る） |
| C | `run_mode` を +1（5 の次は 0 に戻る） |
| B（または電源ボタン 長押し） | 選択中の `run_mode` を実行 |

## モード一覧

| run_mode | 待機中の画面 | B ボタンで実行する動作 |
| --- | --- | --- |
| 0 | 顔（目と口）のアニメーション | MAP読み出しのみ（走行しない） |
| 1 | 壁センサ4個（左前 `line_fl` / 右前 `line_fr` / 左横 `line_l` / 右横 `line_r`）の値とバッテリ残量 | テスト走行 |
| 2 | 保存済みのMAPを描画 | テスト走行（1 と共通） |
| 3 | `Solve the maze` とゴール座標 | 探索走行（足立法） |
| 4 | `fast run` とゴール座標 | 最短走行 |
| 5 | `slam run` とゴール座標 | 最短走行（スラローム強制） |

走行系のモードは、右前センサに手をかざす（`line_fr` が 280 を超える）とスタートします。

### 0：MAP読み出し
EEPROM に保存された壁情報を読み込む（`load_map()`）だけで、走行はしません。

### 1 / 2：テスト走行
尻当て位置から迷路中心まで直進し、さらに半区画＋33mm 進んで前壁の手前で停止します。停止後に前センサ2個（`line_fl` / `line_fr`）の値を表示するので、`parameter.h` のセンサ基準値（`REF_SEN_*` / `END_SEN_*`）の調整に使います。

### 2 の待機画面：MAP表示
`load_map()` で読み出した壁情報を 16x16 で描画します（`view_map()`）。画面は左下がスタート区画 (0,0) です。

| 色 | 意味 |
| --- | --- |
| 赤 | 壁あり |
| 黒 | 壁なし |
| ダークグレー | 未探索 |
| 黄 | ゴール区画周辺の壁、およびスタート区画の壁 |

### 3：探索走行
足立法で `GOAL_X, GOAL_Y` まで探索 → `save_map()` → スタート区画 (0,0) まで戻る探索 → `save_map()` → 180度回頭して MAP を表示、という流れです。その後 `case 99` へ飛び、数秒後に自動で再スタートして重ね探索を繰り返します。

探索中も、既知の壁だけでゴールまでの経路が引ける状態になっていれば `save_map_on_the_way()` で随時 EEPROM に保存されます（EEPROM 書き込み中はタイマ割り込みが止まるため、停止中にのみ実行されます）。

### 4：最短走行
`load_map()` で壁情報を読み出し、`fast_calc()` で経路を計算してから `fast_run()` で走行します。`fotce_sr_en = false` なので、前壁との角度誤差が大きいターンではスラロームせず、半区画進んで超信地旋回する動作に切り替わります。

### 5：最短走行（スラローム強制）
4 と同じですが `fotce_sr_en = true` にします。前壁の状態によらず常にスラロームで曲がります。

### ゴール座標について
ゴール座標は `parameter.h` の `GOAL_X` / `GOAL_Y` で設定します。`ONEONE` を定義すると 1区画、未定義（既定）なら (GOAL_X, GOAL_Y) から (GOAL_X+1, GOAL_Y+1) の 2x2 がゴール区画になります。

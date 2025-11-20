# 初期化処理 / 終了処理
src/core.cpp

## int audio_coreInitialize()
オーディオバックエンド全体を初期化
動作:
すでに AudioBackendContext がセットされている場合、即座に 1 を返す
FMODを初期化して利用できる状態にする
AudioBackendContext をインスタンス化し、グローバル変数にポインターをセットする。
初期化に成功したら 1 を返す。
初期化に失敗したら、エラーの詳細を audioBackendContext.setLastError でセットした後、 0 を返す。

## audio_coreFree()
オーディオバックエンド全体を解放
動作:
FMODの終了処理を行う
audioBackendContext を解放する
グローバル変数に保持していたポインターをクリアする

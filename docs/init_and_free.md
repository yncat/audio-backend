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

# revision 2
関数の戻り値を変更。
成功時: 0
失敗時: -1
これは、他の関数が、正しい値として、スロット番号 0 を返すことがあり、これらの関数の 0 == 失敗 と引数の意味が整合していないため変更する。

# revision 3
ライフサイクルに関連する audio_core** の関数群を main.cpp に移動。 main.cpp の中に、 dll export の関数を全て並べることにより、可読性を向上させる。
元々の関数は同じ名前にすることができないので、 audio_ という prefix を外した形にリネームする。
main.cpp に dll export の関数を移動したので、その中身は core.cpp にある元々の実装をただ呼び出すだけにする。

# AudioBackendContext
src/context.cpp
オーディオバックエンドの状態を表すクラス。
最初のバージョンでは、以下のメンバーを持つ。
string last_error: 最後に発生したエラーの詳細を文字列で保持する。
以下のメソッドを持つ。
SetLastError(string error)
string getLastError()

# グローバルコンテキスト変数
context.cpp の中に、ポインター型の変数として定義。
この変数が not null のとき、バックエンドが起動していることを表す。とりあえず領域の定義だけでよい。

# lastErrorの取得
main.cpp
audio_errorGetLast 関数をエクスポート
動作:
charのメモリアドレスと、サイズを受け取る。
グローバルコンテキストが NULL なら、なにもせずに返る。
指定された領域に、グローバルコンテキストの getLastError() で得られる文字列を書き込む。ただし、 size - 1 バイトまでしか書き込まないようにする。

# サンプルプログラム
サンプルプログラムは変更不要。

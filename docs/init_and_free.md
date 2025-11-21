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

# revision 4
現状、初期化の状態を g_ctx のnull判定で実装しているため、初期化でエラーが起こった際のlastErrorを取得できるために、煩雑なコードになっている。
おそらく、 g_ctx に backend_initialized のようなフラグを持たせれば、初期化失敗時にも g_ctx のポインターを null reset する必要がなくなるので、複雑さが解消されるのではないかと思う。
context.cpp に isBackendInitialized() 関数を追加。
g_ctx == nullptr then return false
g_ctx.isBackendInitialized() == false then return false
return true
(上記２行は値をそのまま戻すことで１行に変換できる)
g_ctx の nullptr 判定をしている箇所を探す。　それらを、 先ほど作成した isBackendInitialized() 関数呼び出しに置き換える。

# revision 5
dll 利用者の利便性向上
通常、 FMOD は、ゲームのマイフレームごとに update 関数を呼び出す必要がある。が、これを dll 利用者の責任で行わせるのはちょっと面倒である。
なので、audiobackendの初期化時に native thread を立てて、そこで update を一定間隔で呼ぶようにする。これにより、ユーザーは裏での処理を意識する必要がなくなる。また、今後の機能追加の際にも、dllの中で常に稼働しているワーキングスレッドを持っておくことは、色々と役立つ。
ワーキングスレッドは、 working_thread.cpp に実装する。
audiobackendの初期化が完了したらスレッドを起動し、バックエンド終了時にスレッドを終了する。FMODの終了処理をする前にスレッドを終了させ、その終了を待ってからFMODを閉じる必要がある。
スレッドの中では、FMODのupdateを30ms間隔で実行する。これで、約30FPSになる。おそらく十分でしょう。
スレッドハンドルはg_contextに保持すればよいと思う。
外部からスレッドを終了させる必要があるので、何らかのイベント機構で、スレッドが終了すべきであることを伝える手段が必要。
最後に、サンプルプログラムではupdate関数を呼ぶ必要がなくなるので、サンプルプログラムとdllからupdate関数を削除。

# bgm
BGM関連は、 bgm.cpp に実装する。

## audio_globalSetBgmVolume(float volume)
bgmの音量を設定する。

## int slot = audio_bgmLoad(address, size)
bgmを読み込む。スロット番号を返す
もし、 FMOD 側がメモリブロックをコピーしないと思われる場合は、 audiobackend 側でメモリバッファをコピーする必要がある。言い換えると、呼び出し側は、メモリブロックをこの関数に提供した後、任意のタイミングで free しても問題ないようにする。

## audio_bgmPause(slot)
BGMを一次停止

## audio_bgmResume(slot)
BGMを再開

## audio_bgmStop(slot)
BGMを止める

## audio_bgmFadeout(slot, ms)
指定の時間をかけてBGMをフェードアウト

## audio_bgmFadein(slot, ms)
指定の時間をかけてBGMをフェードイン

## audio_bgmCrossfade(slot1, slot2, ms)
指定の時間をかけて、BGMをクロスフェードする

## audio_bgmSetLoopPoint(slot, ms)
BGMのループポイントを設定(再生し終わったらループポイントに戻る)

## audio_bgmFree
bgmを解放

# revision 2
BGMに関連する audio_bgm** の関数群を main.cpp に移動。 main.cpp の中に、 dll export の関数を全て並べることにより、可読性を向上させる。
元々の関数は同じ名前にすることができないので、 audio_ という prefix を外した形にリネームする。
main.cpp に dll export の関数を移動したので、その中身は bgm.cpp にある元々の実装をただ呼び出すだけにする。

## revision 3
サンプルプログラムの変更。
現在: その場で生成した波形データを使用している。
変更後: BGM1として assets\bgm_full.ogg を利用する。 BGM 2 として、 assets\bgm_hats.ogg を使用する。

# revision 4
エラーハンドリングの改善。
bgm.cpp の中で、戻り値がないものに対して、必ず int を返すように変更。これは、処理成功時に0を返し、失敗時に-1を返す。
-1を返す場合、 g_ctx.last_error にエラーの内容を英語で書き込む。
関数が int を返すようになったので、 dll export の関数シグネチャを変更し、 int を返せるようにする。
サンプルプログラムの変更: bgmの操作後にエラーチェックを行う。-1が返ってきていたら、getLastErrorでエラーメッセージを取得してコンソールに表示する。エラーが発生した場合は、テストを終了してメニューに戻る。

# revision 5
bgmPlayの関数を作り忘れてたので追加する。
bgm.cpp: bgmPlay関数を追加。指定されたスロットのBGMを再生する。再生する前に、再生位置を0にリセットする。
main.cpp: dll entrypointを追加。 audio_bgmPlay
サンプルプログラムの変更: 最初にBGMを再生するところが、現在fadeinになっている。fadeinは実質crossfadeでテスト可能なので、最初の再生にbgmPlayを利用するように修正すればよい。

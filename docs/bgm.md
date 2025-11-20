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

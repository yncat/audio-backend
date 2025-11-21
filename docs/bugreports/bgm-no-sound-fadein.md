# BGM No Sound on Fadein

## Commit Hash
f3d168d3779158fd221e6ba05186fbef3d2fadec

## Bug Report
サンプルプログラムのBGMテストで、BGMが一切流れない

## Cause
`bgmFadein`関数内で、フェードイン開始前に`channel->setVolume(0.0f)`を呼び出していた。

FMODのドキュメントによると、`addFadePoint`で設定されるフェードポイントの音量は`setVolume`の値に**乗算**される（"Fade points are scaled against other volume settings"）。

そのため、`setVolume(0.0f)`が設定されていると、フェードポイントがどのような値であっても最終的な音量は常に0になり、音が再生されなかった。

## Fix
`bgm.cpp`の`bgmFadein`関数から`channel->setVolume(0.0f)`の呼び出しを削除した。

また、以下の追加修正も行った：
- `audio_coreUpdate()`関数を追加（FMODの`System::update()`を呼び出す）
- サンプルプログラムで定期的に`audio_coreUpdate()`を呼び出すように変更

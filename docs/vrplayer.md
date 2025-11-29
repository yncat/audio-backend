# プレイヤー位置情報の更新
プレイヤーの情報は、以下の２つのインターフェイスで更新できるようにする。
audio_vrPlayerSetPosition(float width, float depth, float height)
audio_vrPlayerSetRotation(UnitVector3d front, UnitVector3D up)

# UnitVector3dの追加
src/vrstructs.h
Position3dを丸ごとコピーしてUnitVector3dを作成

# プレイヤー位置情報の変更関数を追加
src/vrplayer.cpp
ここにsetPlayerPositionとsetPlayerRotationを追加
global contextにあるプレイヤーの3d情報を更新する
必要な単位の変換などはこの関数内で行う
setPlayerPositionを実行したとき、 player_sounds チャンネルグループにある位置情報を、プレイヤーと同じものに更新する必要がある。

# dll export 追加
main.cpp
上記２つの関数を、それぞれ
audio_vrPlayerSetPosition
audio_vrPlayerSetRotation
としてdll exportに追加

# プレイヤー動作サウンド
プレイヤーが出す音は、reverbを適用する必要があるため、resonance audioを経由して再生する。ただし、プレイヤーの位置と同じ場所から再生する。

# VR 初期化時の処理に追加
"player_sounds" というチャンネルグループを追加
ここに resonance audio listener をアタッチしておく

# vrOneshotPlayer を追加
src/vrobj.cpp
vrOneshotPlayer 関数を追加
sample_key と sound_attributes を受け取る
player_sounds のチャンネルグループの子供としてサウンドを生成し、再生する。親にresonanceがアタッチされているので、soundAttributesを適用して再生すればよい。soundAttributesのpanは無視する。

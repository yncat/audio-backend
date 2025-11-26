# 3Dオブジェクト
3Dオブジェクトは、 resonance audio による立体音響で再生される。

# 座標系
width: FMODの座標系では X 方向。
depth: FMODの座標系では Z 方向。
height: FMODの座標系では Y 方向。

# 位置情報構造体
src/vrstructs.cpp
3Dポジションの構造体を作成。
- int width
- int depth
- int height

プレイヤー位置情報の構造体を作成。以下の４つを持つ
  FMOD_VECTOR pos,
  FMOD_VECTOR vel,
  FMOD_VECTOR forward,
  FMOD_VECTOR up

# ctxにリスナーの情報を保持
リスナーは一人であると考えてよい。
ctx に vr_listener_attributes を追加
初期値は、真っ正面を向いている状態。 pos=0,0,0 vel=0,0,0 forward=0,0,1 up=0,1,0
この状態で System::set3DListenerAttributes も呼び出しておく。

# 3dワンショット再生
src/vrobj.cpp
3D空間上で、ワンショットでサウンドを再生する
- vrOneshotRelative(sample_key, position3d, sound_attributes, follow bool): リスナー位置を基準とした相対座標でワンショット再生
- vrOneshotAbsolute(sample_key, position3d, sound_attributes): ワールドの絶対座標でワンショット再生
処理は以下の通り。
vrInitialized == false ならエラー
3Dサウンドは VR というチャンネルグループに入れないと行けない
VR チャンネルグループの中で、指定されたサンプルキーのサンプルからサウンドを生成。この時に 3D フラグを付ける。最初は paused の状態にする。
relative の呼び出し、かつ、 follow=true の場合、 FMOD_3D_HEADRELATIVE フラグを付けた状態でサウンドを生成する。
position3d を使って位置情報を設定。この際、 width を x にマッピング、 depth を z にマッピング、 height を y　にマッピングする必要がある。
absolute の呼び出しでは、引数の座標をそのまま設定。relative の呼び出しでは、ctxからリスナー位置を取得し、そこから相対的にワールド座標を計算してセットする。
sound_attributes にセットされている値を反映する。なお、 pan は無視してかまわない。
最後にサウンドを再生する。

# dll exportの追加
src/main.cpp
audio_vrOneshotRelative と audio_vrOneshotAbsolute として追加

# サンプルプログラムの変更
3d oneshot test を追加。
assets/ding.wav をいろんな設定で再生していく。
- 正面 (0, 0, 3)
)
- 右前(2, 0, 2)
- 右(3, 0, 0)
- 以降時計回りに一周
しばらく待って
今度はピッチを0.8にしてまた一周
またすこし待って
今度はボリュームを0.3にしてまた一周

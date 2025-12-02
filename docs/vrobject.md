# VRオブジェクト
ゲーム内のオブジェクトを表す。複数のサウンドソースを持つことができ、3D空間上に配置される。

# 管理方法
context に unordered_map<string, VRObject> で一覧を管理する。
string はキー名で、dllの呼び出し元が決定して指定する。VRオブジェクトを操作するときは、キーを使って参照する。

# VRObject struct
src/vrobj.h および src/vrobj.cpp
以下のメンバーを持たせる。
position3D center
size3d サイズ
FMOD_sound *looped_sound
FMOD_ChannelGroup *channel_group

# VRObjectInfo struct
VRオブジェクトをやり取りする際の情報伝達用構造体。
position3d position
size3d size
const char *looped_sample_key

# int result = vrObjectAdd(string key, VRObjectInfo *info)
指定した情報を使ってオブジェクトを登録する。エラーなら-1を返し、正常終了なら0を返す。
キー名がすでに使われていたらエラー。
looped_sample_keyが指定されており、かつ、それに対応するsampleが読み込まれてなかったらエラー。
VRObject構造体を初期化
FMOD channelgroupを１つ作成し、構造体に格納
position, sizeを構造体に格納。
positionの値を使って、チャンネルグループの位置情報を設定 (resonance audio パラメータ直接流し込む)
loopedSampleKey が指定されている (not null) 場合、FMOD_Sound を looped モードで生成し、VRObject構造体のlooped_sound_sourceに格納

# int result = vrObjectRemove(const char *key)
キー名でVRオブジェクトを探し、あればremoveする。なかったらエラーにする。
removeするときに、対象オブジェクトのchannelGroupを破棄する。

# int result = vrObjectStartLooping(const char* key)
指定したオブジェクトのループサウンドを再生する。再生する前に、再生位置を先頭にシークする。

# int result = vrObjectPauseLooping(const char* key)
指定したオブジェクトのループサウンドを一次停止する。

# int result = vrObjectResumeLooping(const char* key)
指定したオブジェクトのループサウンドを再開する。再生位置をリセットしないバージョン。

# int result = vrObjectPlayOneshot(const char *objectKey, const char *sampleKey, soundAttributes *attributes)
objectKey で指定したオブジェクトから、ワンショットサウンドを再生する
他のワンショット再生と同様に、使い捨てのサウンドを生成してワンショットを再生するが、チャンネルグループの親がオブジェクトのチャンネルグループになるようにする。これにより、オブジェクトの場所から音を出せる。

# dll exportの追加
src/main.cpp
audio_vrObject** の形で dll export を追加する。

# サンプルプログラムの変更
VR object test を追加
以下のサンプルを読み込み
- assets/missile.ogg
- assets/explosion.ogg
- assets/gunloop.ogg
- assets/gunend.ogg
5, 3, 0 の位置に、 "gun" というキー名でオブジェクトを追加。looped_sample_keyをgunloopに設定。
vrObjectStartLoopingでループ再生を開始
３秒ぐらい待つ
missileというオブジェクトを、gunと同じ場所に生成。こちらのループ音はなし。
missile オブジェクトに対して、 oneshot で missile.ogg のサンプルを再生
missile のオブジェクトのx座標を移動させていく
150msずつ待って、x座標を１ずつ減らしていく。これを１０回行う。
通常のワンショット再生を使い、 explosion.ogg を -5, 3, 0 の位置から再生
missile オブジェクトをremoveする
３秒待つ
gun オブジェクトに対して oneshot再生を呼び出す。 gunoff.ogg のサンプルを再生する。
直後に gun のオブジェクトに対して pauseLooping を呼び出してループ音を止める。
２秒待つ
gun オブジェクトをremoveして終了する

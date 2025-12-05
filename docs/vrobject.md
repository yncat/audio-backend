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

# revision 2
オブジェクトの位置情報を設定する vrObjectChangePosition(const char *key, position3d pos) を追加
サンプルプログラムの修正:
今、 150ms が使えないからといって、 waitSecond が 0 に設定されている。これはだめです。 waitMilliseconds も作って、150msしっかり待つようにする。そのうえで、ミサイルが移動するところで、オブジェクトの位置情報をちゃんと設定するように変更。

# revision 3
範囲の広いオブジェクトの3D空間化を考える。
たとえば、中心座標が x: 5, y: 0, z: 1.5 の場合、その場所を中心として音が出るだけである。そうすると、たとえばこのオブジェクトが横に長い形をしていたとしても、中心の１点からしか音が出ないということになる。
そのため、今、 vrobject.cpp で使っている VRObject 構造体では、 size というフィールドを用意している。
今回やりたいのは、サイズが0より大きい場合、プレイヤーとの位置関係に応じて、実際に音を出す位置を微調整する処理である。

## 構造体にメンバーを追加
bool is_wide を VRObject 構造体に追加する。
position3d sound_position フィールドも追加する。これは、サウンドを再生する実際の位置を格納するフィールドとなる。

## 処理
まず、オブジェクトを add する段階で、size の各値が0より大きいかどうかをチェックする。１つでも 0 より大きい要素があった場合、 is_wide フラグを立てる。
次に、 wide になったオブジェクトの位置を計算するための実装を追加する。
この実装は vrplayer.cpp と vrobject.cpp から参照される可能性が高いため、 vrpositioning.cpp という別のファイルに分離する。
以下のシグネチャを持つ関数を追加する。
void updateObjectSoundPosition(VROBJECT *obj, position3d *playerPos)
処理内容は以下の通りとする。
obj.is_wide == false のとき、 center_positionの要素をsound_positionにコピーしてreturnする。
ここからは wide のときの処理。
次の処理として、オブジェクトの中にプレイヤーが入っている場合、プレイヤーの位置情報をそのままsound_positionにコピーしてreturnする。この判定は、center_positionとsizeを使ってオブジェクトの範囲を計算し、その中にプレイヤーの座標が含まれているかどうかで判定する。
これで解決しない場合、座標軸ごとにどこをsound_positionとするか計算していく必要がある。
マズx軸の判定をする。
オブジェクトのx軸の範囲よりもプレイヤーのx座標が小さい場合、オブジェクトのx座標の最小値をsound_xとする。
オブジェクトのX軸の範囲内にプレイヤーのX座標が入っている場合、プレイヤーのx座標をsound_xとする。
オブジェクトのx軸の範囲よりもプレイヤーのx座標が大きい場合、オブジェクトのx座標の最大値をsound_xとする。
これを言い換えると、「座標の範囲に入っていればプレイヤーと直線上にある座標を使い、そうでなければオブジェクトの端っこを座標として採用する」ということになる。
上記の例をY座標とZ座標に対しても適用する。
最終的に３つの軸に対するsound_positionを書き換えたあとでreturnする。

## 具体例
object: center_position x 5.0 y 0.5 z 1.5, size: x 5.0, y 0.5, z 1.5
実際のオブジェクト範囲:
左、手前、下の角: 0.0, 0.0, 0.0
右、手前、下の角: 10.0, 0.0, 0.0
左、奥、下の角: 0.0, 0.0, 3.0
右、奥、下の角: 10.0, 0.0, 3.0
左、手前、上の角: 0.0, 0.0, 1.0
右、手前、上の角: 10.0, 1.0, 0.0
左、奥、上の角: 0.0, 1.0, 3.0
右、奥、下の角: 10.0, 1.0, 3.0
この範囲内にいる場合: プレイヤーの位置と同じところに音源を配置
-1.0, 0.0, 1.0にいる場合: 左の壁が直線上にあるため、 左の壁である 0.0, 0.0, 1.0に音源を配置
-3.0, 0.0, -3.0にいる場合: どの辺にも接触していない、一番近い角は左、手前、下の角のため、 0.0, 0.0, 0.0 に音源を配置

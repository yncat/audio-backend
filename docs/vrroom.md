# VRオーディオのルームエフェクト
ルームの情報をdllに登録して、スロット番号を指定して切り替える

# 変換層の追加
src/adapter_resonance.cpp
resonance_room_properties.hをインクルードしておく
ここに、 convertToResonanceMaterialEnum関数を追加
char *material_nameを受け取る
以下の変換表に従って、 resonance_room_properties.hの k から始まる値に変換する

transparent
acoustic_ceiling_Tiles
brick_bare
brick_painted
concrete_block_Coarse
concrete_block_painted
curtain_heavy
fiber_glass_insulation
glass_thin
glass_thick
grass
linoleum_on_concrete
marble
metal
parquet_on_concrete
plaster_rough
plaster_smooth
plywood_panel
polished_concrete_or_tile
sheetrock
water_or_ice_surface
wood_ceiling
wood_panel
Uniform

どれにも変換できなかったら -1 を返す。

次に、ルームの抽象化データからresonance audioのroomPropertiesに変換する関数を実装。
storedRoomのcenterPositionをresonanceのroom positionに変換
rotationは回転なしで固定
壁の素材を設定（この際に素材の文字列->resonance enumへの変換が必要になる)
その他の反射係数などの値はデフォルトのままにする

# 構造体の追加
vrstructs.h
WallMaterials 構造体を追加
６つの char *を持つ。
front
back
left
right
floor
ceiling
storedRoom という構造体を作成
この中には、 centerPositionとWallMaterialsを持つ

# roomの保管場所
contextに保持する。
storedRoomをvectorで複数持てるようにする

# ルーム関係の関数
int roomIndex = roomAdd(position3d centerPosition, wallMaterials materials)
部屋を定義する。wallMaterialsの受け取りをポインター型にしていた場合は、呼び出し元がそのポインターを解放してもよいように、コピーを取っておく。
storedRoomを作成して、contextのvectorに追加
追加したインデックス番号を返す。

roomChange(int index)
指定したインデックス番号のルームをcontext vectorから取得
adapter_resonanceの関数でroomProperties構造体に変換
それを、resonance audio listener のパラメータindex1に設定

RoomClear()
ルームのvectorをクリアする。resonance audio listener のパラメータは、全てデフォルトの roomProperties で上書きする。

# サンプルプログラムの変更
room effect test を追加
ルーム１とルーム２を適当に追加
assets\clap.wavを再生
まずはどのルーム設定も適用してない状態で再生
room1に切り替えて再生
room2に切り替えて再生
roomClearを実行
もう一度clapを再生



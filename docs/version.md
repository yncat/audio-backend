# バージョン情報取得
dll のバージョンは、 major, minor, patch の semantic versioning で管理する。
src/version.cpp で、これら３つの定数を定義。
最初は 1.0.0 とする
以下の関数を実装
int getMajorVersion
int getMinorVersion
int getPatchVersion

src/main.cpp
dll呼び出しようとして、こちらに以下の関数を追加
audio_versionGetMajor
audio_versionGetMinor
audio_versionGetPatch
内部では、 version.cpp で実装されている関数を呼び出すようにする。

# サンプルプログラムの変更
サンプルプログラム起動時に、dllのバージョンを取得して表示する。

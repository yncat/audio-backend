# plugin inspector
FMODのプラグインを読み込むことはできるが、何個のプラグインがネストされており、どんなパラメータがあるのかマニュアルがないことが多い。
それを調べてファイルに書き出す機能を作りたい。

# plugin inspectorの実装
plugin_inspector.cpp に実装。
int inspectFmodPlugin(char *path, char *outfilepath)
backendが初期化されてなかったらエラー。
指定されたパスのプラグインをFMOD出読み込む。
nested pluginの数を調べて、その数だけループ。
ネストされているプラグインを１つずつ読み込む。
名前を取得。
パラメータリストを取得。
各パラメータの情報を取得。
それらをファイルに書き出す。
指定された出力ファイルに書き出す。
プラグインを読み込み解除。
成功したら0を返す。
main.cpp に audio_corePluginInspect として登録。

# サンプルプログラムに追加
plugin inspector test を追加
プラグインのパスを入力
出力ファイル名を入力
プラグインインスペクターを実行。

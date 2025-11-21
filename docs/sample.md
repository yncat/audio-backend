# sample
sample とは、音声ファイルの実態を指す。
sample から sound を生成して、それを再生する。つまり、同じサウンドならば、複数あっても同じサンプルデータのメモリを再利用する。
ここでいう「サウンド」は、 FMOD でいうところにの「channel」に該当する。

## int audio_sampleLoad(address, size, key)
オーディオサンプルをロード
事前にデコードしてほしいため、 FMOD_CREATESAMPLE でロードする。
この場合、デコードした結果は FMOD の内部メモリにコピーされるので、 audiobackend でバッファのコピーを取る必要はない。
g_ctx に samples_map という unordered_map を作成できるようにする。
ここには、 key から sample にアクセスできるmapを構築する。
最初に、同じキーでマップに登録されてないか検査し、登録されていたらエラーにする。
サンプルを読み込み、mapを更新する。
成功したら0、失敗したら-1を返す。
失敗時には、失敗の理由を g_ctx.lastError に記録する。

## audio_sampleOneshot(char *key, attributes *sound_attributes)
keyで指定したサウンドをワンショット再生。完全に使い捨て。
sound_attributesに入っている値をそのまま設定。FMODの薄いラッパーを意識しているため、独自の変換などをする必要は一切ない。

# サンプルプログラムの追加
サンプルプログラムのメニューに「sample and oneshot test」を追加。
assets\ding.ogg をsampleとして読み込む。
中央、音量100%、ピッチ100%で再生
パンや音量やピッチを変えて何度か再生して終了。


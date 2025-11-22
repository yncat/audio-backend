# 3Dオーディオの初期化
3Dオーディオを実装していきます。
このドキュメントでは、3Dオーディオの初期化について示します。
今回は、 Google の Resonance Audio という FMOD プラグインを利用します。

1. FMOD low-level APIでのDSPプラグイン読み込み方法                                                          
FMODでは2つの方法でプラグインを使用できます：                                                               
  - 動的リンク: System::loadPlugin("resonanceaudio.dll", &handle) でランタイムに読み込み                      
  - 静的リンク: System::registerDSP() でプラグインのディスクリプタを登録                                      
今回は、動的リンクを使います。

2. Resonance Audioプラグインの構成                                                                          
3つのDSPコンポーネントがあります：                                                                          
- Resonance Audio Listener: Master Busに配置、バイノーラルレンダリングとルームエフェクトを処理              
- Resonance Audio Source: 各音源に配置、モノラル音源の空間化                                                
- Resonance Audio Soundfield: FOA (First Order Ambisonic) サウンドフィールドの再生                          

3. 連携の鍵となるAPI                                                                                        
// プラグインの読み込み                                                                                     
System::loadPlugin(const char *filename, unsigned int *handle)                                              
// DSPの作成                                                                                                
System::createDSPByPlugin(unsigned int handle, FMOD::DSP **dsp)                                             
// ChannelGroupやChannelに追加                                                                              
ChannelControl::addDSP(int index, FMOD::DSP *dsp)                                                           
// パラメータ設定                                                                                           
DSP::setParameterFloat(int index, float value)                                                              
DSP::setParameterData(int index, void *data, unsigned int length)                                           

今回の実装範囲:
とりあえず以下の部分まで実装してください。
     │ 1. resonanceaudio.dll の配置と読み込み処理                                      │                      
     │ 2. 初期化時にプラグインを System::loadPlugin() で読み込み                       │                      
     │ 3. 3Dサウンド用のチャンネルグループを作成し、Listener DSPをそこに追加                               │                      

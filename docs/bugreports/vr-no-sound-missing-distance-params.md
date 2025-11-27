# VR Audio No Sound - Missing Distance Attenuation Parameters

## Commit Hash
a2e96c6e8501ae10dbb904ab06fa3cceff3fdea8

## Bug Report
VR音声（立体音響）の再生が全く聞こえない。VR初期化は成功するが、`audio_vrOneshotRelative()`および`audio_vrOneshotAbsolute()`で音を再生しても無音のまま。エラーは発生せず、関数は正常に完了する。

## Investigation Process
動作している AGPP-game プロジェクトの Oculus Spatializer 実装と比較調査を実施：

### AGPP (Oculus Spatializer) の実装
- チャンネルグループに DSP を追加
- DSP 作成後、以下のパラメータを明示的に設定：
  - `OSP_PARAM_INDEX_ATTENUATION_ENABLED`: true
  - `OSP_PARAM_INDEX_SOURCE_RANGE_MIN`: 0.5f
  - `OSP_PARAM_INDEX_SOURCE_RANGE_MAX`: 200.0f

### このリポジトリ (Resonance Audio) の実装
- 個々のチャンネルに DSP を追加
- 3D 位置情報（parameter index 8）のみ設定
- **距離減衰パラメータが未設定**

## Root Cause
Resonance Audio Source DSP に対して、距離減衰パラメータ（Min Distance、Max Distance）が設定されていなかった。

これらのパラメータが設定されていない場合、デフォルト値が使用される可能性があるが、適切な空間音響処理が行われず、結果として音が聞こえない状態になっていた。

AGPP の Oculus Spatializer では距離減衰を明示的に有効化・設定していたことから、Resonance Audio でも同様のパラメータ設定が必要であることが判明。

## Fix Details
`src/vrobj.cpp` の `vrOneshotRelative()` および `vrOneshotAbsolute()` 関数で、Source DSP を作成した後、3D 位置情報を設定する前に距離減衰パラメータを追加：

### 追加したパラメータ設定
```cpp
// Set distance attenuation parameters (similar to AGPP implementation)
// Parameter [2]: Min Distance
result = sourceDsp->setParameterFloat(2, 0.5f);
if (result != FMOD_OK) {
    g_context->SetLastError(std::string("Failed to set min distance: ") + FMOD_ErrorString(result));
    sourceDsp->release();
    channel->stop();
    return -1;
}

// Parameter [3]: Max Distance
result = sourceDsp->setParameterFloat(3, 200.0f);
if (result != FMOD_OK) {
    g_context->SetLastError(std::string("Failed to set max distance: ") + FMOD_ErrorString(result));
    sourceDsp->release();
    channel->stop();
    return -1;
}
```

### パラメータの意味（`docs/resonance_audio_parameters_list.md` より）
- **Parameter [2]: Min Distance** (Float, range: 0 to 10000 m)
  - 音源からこの距離までは音量が一定に保たれる
  - 設定値: 0.5m
- **Parameter [3]: Max Distance** (Float, range: 0 to 10000 m)
  - 音源からこの距離以降は音量が最小値になる
  - 設定値: 200.0m

### 修正箇所
1. `src/vrobj.cpp:111-128` - `vrOneshotRelative()` 関数
2. `src/vrobj.cpp:254-271` - `vrOneshotAbsolute()` 関数

## Result
修正後、VR 音声が正常に再生され、立体音響効果が確認できるようになった。リスナーの周囲360度から音が聞こえ、位置による音量・定位の変化も正しく動作している。

## Lessons Learned
- プラグインの DSP パラメータは、ドキュメントに明記されていなくても、距離減衰などの重要なパラメータを明示的に設定する必要がある
- 動作している類似実装（今回は AGPP の Oculus Spatializer）との比較調査が、問題解決の有効な手段となる
- FMOD の空間音響プラグインでは、3D 位置情報だけでなく、距離減衰パラメータの設定も必須である可能性が高い

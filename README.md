# Audio Backend DLL

ゲームのオーディオ処理をDLLにまとめたプロジェクトです。C++で実装され、シンプルなC APIを通じてGoから呼び出せるようにします。

## プロジェクト構成

- `src/` - ソースコード
- `lib/` - ライブラリファイル
- `docs/` - ドキュメント
- `bin/` - ビルド出力

## ビルド方法

### 前提条件

- Visual Studio (MSVC toolchain)
- x64 Native Tools Command Prompt for VS

### audiobackend.dll のビルド

1. **x64 Native Tools Command Prompt for VS** を起動
2. プロジェクトのルートディレクトリに移動
3. 以下のコマンドを実行:

```bash
build_dll.bat
```

ビルドが成功すると、`bin\audiobackend.dll` が生成されます。

### audiobackend_examples.exe のビルド

サンプルプログラムをビルドするには:

```bash
build_examples.bat
```

ビルドが成功すると、`bin\audiobackend_examples.exe` が生成されます。

## サンプルプログラムの実行

```bash
cd bin
audiobackend_examples.exe
```

メニューから選択して各種機能をテストできます。

## 技術仕様

- 64-bit DLL/アプリケーション
- FMOD のラッパーとして実装
- Google Resonance Audio プラグインを使用した Spatial Audio サポート
- 全ての関数は non-blocking で動作
- メモリからの音声ファイルロード（暗号化対応のため）

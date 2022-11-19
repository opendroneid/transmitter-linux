
# OpenDroneID サンプル送信機 for Linux

本プログラムは、デスクトップLinux PCを用いてWi-Fi BeaconまたはBluetoothによるリモートID静止データの送信をサポートします。

OpenDroneIDと関連する仕様の詳細については [opendroneid-core-c](https://github.com/opendroneid/opendroneid-core-c) のドキュメントを参照してください。

この主目的は、有効なリモートIDデータを送信するためのBluetoothおよびWi-Fi Beacon ハードウェア＋ソフトウェアのセットアップ方法を実証するためであり、リモートIDデータそのものは単なる静的データとなります。
これらはもう少しダイナミックな飛行例をシミュレートするため、簡単に拡張することも可能です。

## コンパイル方法

以下の手順は、Ubuntu 20.04 と Raspbian 10 (Buster) / Rasbian 11 (Bullseye) でテストされています。  

```
git clone https://github.com/opendroneid/transmitter-linux.git
cd transmitter-linux
git submodule update --init
```
Bluezのソースコードリポジトリは、Ubuntu 20.04にインストールされているバージョンと一致させているため、Version 5.53に設定されています。
お使いの環境にインストールされているBluezバージョンを確認し、それに合わせて調整する必要があるかもしれません:
必要となる場合は `btmon --version`, `btmgmt --version`, `bluetoothctl --version`, `hcitool` or `hciconfig` のいずれかのバージョンを確認してください。

hostapdをコンパイル:

```
sudo apt install build-essential git libpcap-dev libsqlite3-dev binutils-dev bc pkg-config libssl-dev libiberty-dev libdbus-1-dev libnl-3-dev libnl-genl-3-dev libnl-route-3-dev
cd hostapd/hostapd
cp defconfig .config
make -j4
cd -
```

gpsdをコンパイル (すべてのビルド要件は [gpsd/build.adoc](https://gitlab.com/gpsd/gpsd/-/blob/master/build.adoc) をご参照ください):
```
sudo apt install scons
cd gpsd
sed -i 's/\(variantdir *=\).*$/\1 "gpsd-dev"/' SConstruct
scons minimal=yes shared=True gpsd=False gpsdclients=False socket_export=yes
cd -
```

送信機サンプル・アプリケーションをコンパイル:
```
mkdir build && cd build
cmake ../.
make -j4
```

## コマンドライン パラメータ

* `b` Wi-Fi Beacon 送信の有効化
* `l` Bluetooth 4 Legacy Advertising 送信の有効化 (非拡張 Advertising API)
* `4` Bluetooth 4 Legacy Advertising 送信の有効化 (拡張 Advertising API)
* `5` Bluetooth 5 Long Range + Extended Advertising 送信の有効化
* `p` シングルメッセージの代わりにメッセージパックを使用
* `g` gpsdを使用して、メッセージの各ループ後に位置情報メッセージを動的に更新

## Wi-Fi Beacon 送信の開始

Wi-Fi Beacon 送信は、PCがWi-Fiホットスポットに接続されていないときのみ正常に動作します。
接続されている場合は、これらの方法でWi-Fiハードウェアがローカルアクセスポイントとして動作するよう設定することができません。

Ubuntuの場合は【system settings -> Wi-Fi】を開き、PCに保存されている各ネットワーク情報に対して歯車アイコンをクリックし【forget network】をクリックします。

Raspberry Pi OS上は、まず右上の【Wi-Fi】アイコンをクリックしてください。
すでに設定されている各ネットワーク情報に対して、右クリックしネットワーク情報を消去してください。
(Raspbian 11 Bullseyeでは【Network】を左クリックすると、情報を消去するかどうか聞いてきます)
また `/etc/wpa_supplicant/wpa_supplicant.conf` ファイルに接続設定が保存されている場合があります。
ファイルを編集し、既知のWi-Fiネットワーク情報をすべて削除/コメントアウトしてから再起動してください。

`rfkill`というツールを実行して、Wi-Fiハードウェアの使用を妨げる状態でないことを確認してください。
ソフトウェアとしての制止は、同一ツールで解除できるはずです。

hostapdを起動するには、以下の内容の設定ファイルが必要です。
例: プロジェクトのルートフォルダに用意されている beacon.conf を修正します。
2行目があなたのWLANデバイスの名前と一致することを確認してください。
例えば `ip link show` や `iw dev`、`ifconfg` 等で確認できます:  

```
country_code=JP
interface=wlan0
ssid=DroneIDTest
hw_mode=g
channel=6
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=0
wpa=2
wpa_passphrase=thisisaverylongpassword
wpa_key_mgmt=WPA-PSK
wpa_pairwise=TKIP
rsn_pairwise=CCMP
ctrl_interface=/var/run/hostapd
beacon_int=200 # Does this work? Doesn't seem to have any effect?

#This is an empty information element. dd indicates hex. 01 is the length of the data and 00 the actual data:
vendor_elements=dd0100

#This information element has one fixed Location message:
#vendor_elements=dd1EFA0BBC0D00102038000058D6DF1D9055A308820DC10ACF072803D20F0100
```

プロジェクトのトップで別のシェルを開き、hostapdデーモンを起動します。
これは `sudo` 権限がある場合のみ動作します:
```
sudo hostapd/hostapd/hostapd beacon.conf
```

元のシェルで、opendroneid transmitterを起動します。
繰り返しますが、これは `sudo` 権限があるときのみ動作します:
```
sudo ./transmit b p
```

これらは、マザーボードに Wi-Fiハードウェアを内蔵した [CometLake Z490 desktop](https://rog.asus.com/motherboards/rog-strix/rog-strix-z490-i-gaming-model) でテストされています。  
何らかの理由により、hostapdに送信されるかなりの量のメッセージが、下位のソフトウェアレイヤによって受信されないか、少なくとも正しく認識されないことがあります。
これはコマンドライン出力をたどることで明確となります。
何故このようなこととなるのかは不明です。

またbeacon.confファイルでBeaconの間隔を設定しても効果がないようで、これらは不明な点となっています。

Raspberry Pi 3Bと4Bでテストしたところ、Beacon送信は一部しか動作しませんでした。
beacon.confファイルでコメントアウトできる、あらかじめ定義された単一な位置メッセージは問題なく動作します。
`transmit` プログラムがhostapデーモンに書き込もうとするデータは、単一のメッセージ (`sudo ./transmit b`) は問題なく動作しますが、メッセージパック (`sudo ./transmit b p`) は動作しません。
Wi-Fiデバイスドライバでベンダ固有の要素データに対して何らかのサイズ制限があるかも知れません。
Raspberry Pi 3B と Raspbian 11 Bullseye でテストしています。
リモートID規格では、Wi-Fi Beaconの送信にメッセージパックを使用することが義務付けられていますのでご注意ください。

## Bluetooth 送信の開始

このプログラムは `sudo` 権限で実行する必要があります:
```
sudo ./transmit 5 p
```

教育目的として `sudo btmon` というツールを使い、HCIのコマンドフローを追跡することができます。
その為にはBluezがインストールされている必要があります: `sudo apt install bluez`

これは、マザーボードに Bluetoothハードウェアを内蔵した [CometLake Z490 desktop](https://rog.asus.com/motherboards/rog-strix/rog-strix-z490-i-gaming-model) でテストされています。
このハードウェアでは、Extended Advertising APIを使用したBT4 Legacy Advertisements (`option 4`) とBT5 Long Range + Extended Advertisements (`option 5 p`) は、個別に実行すれば問題なく動作しました。
しかし、両方を同時に設定すると、何らかの理由でハードウェア / デバイスドライバソフトウェアはBT4の信号のみをブロードキャストするようになりました。

なお、米国でリモートIDにBluetooth送信を使用する場合、BT4とBT5の両方 [同時](https://github.com/opendroneid/opendroneid-core-c#relevant-specifications) 送信することが義務付けられますのでご注意ください。
またヨーロッパや日本では義務化されていませんが、受信機との互換性や電波の到達距離を最大化するために、それら地域でも同様な状態にすると良いでしょう。

BT4 Legacy Advertising non-Extended Advertising HCI コマンド (`option l`) を使用すると、何故かうまくいきませんでした。

Raspberry Pi 3B と 4B でテストしたところ、Extended Advertising HCI インターフェイスコマンドはサポートされませんでした。 (`option 4` および `option 5 p`)
おそらく、ハードウェア / デバイスドライバがBT4 Legacy Advertisingのみをサポートしていることが原因です。
古いBT4 Legacy 非拡張 Advertising HCIコマンド (`option l`) は問題なく動作しました。

【ONVIAN】ブランドのBT5 USBアダプター/ドングルとRTL8761Bチップセット搭載のUbuntu 20.04インストールPCでテストをし、ロングレンジモードで正常に送信できることが確認されました。

## クリーンアップの方法

プログラムが異常終了しても、BeaconとBluetoothのブロードキャストは継続されます。
* Beaconブロードキャストを停止するには `hostapd` インスタンスを停止してください。
  `transmit`インスタンスを停止するのは難しいかもしれません。 `hostapd` を停止した後 `sudo pkill transmit` を使用してください。
* Bluetoothを停止するには `sudo btmgmt power off` 及び `sudo btmgmt power on` を使用してください。


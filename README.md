# IoT-Smoking-Mgmt-System
> 본 시스템은 흡연구역에 대한 집중적인 관리와 간접 흡연에 대한 피해를 방지하고자 개발하였습니다.

> [ 사용된 센서 및 보드 ]
>* Raspberry Pi 3 = *Client*
>* Pi Camera
>* Dot Matrix
>* A to D Convertor
>* MQ-2 Gas Sensor
>* Artik 520 = *Server*

### 기능 정리
> **Client**
> 1. Client를 담당하는 Raspberry Pi는 각각의 센서들에 대한 관리를 주로 담당하게 된다.
> 2. 위험을 감지하게 되면 Server에게 Data를 전송하고 경고등과 위험이 발생한 현장 사진을 찍게 된다.
>* Client <-> Server 연결 기반은 **TCP**를 기반으로 한다.

> **Server**
> 1. Server를 담당하는 Artik 520는 Client로부터 수집된 정보를 저장하고, 해당 data에 대한 log를 남긴다
> 2. 실제 현장에서 관리실을 담당하며, 이상이 발생시 관리실의 경고등을 울리게 한다.
> 3. Client로부터 경고를 받게되면, 즉시 Cloud로 data를 전송하게 된다.
>* Server <-> Cloud 연결 기반은 **Node-js**를 기반으로 한다.

> **Cloud**
> 1. Cloud는 Server로부터 받은 정보를 수집하고, 저장한다.
> 2. 흡연량이 위험한 수치에 이르게 되면, 관리자에게 즉시 알림 문자를 보내게 된다.
> 3. data에 대한 수치량을 GUI로 표현하고 언제든 관리자가 볼 수 있게 한다.

### System Flow Chart 
<div>
<img src="https://user-images.githubusercontent.com/34855745/53615024-4fae8180-3c1f-11e9-9c4a-a6d927e81dc3.JPG" width="800" height="400">
</div>

### Sensor 및 Board의 기능 정리
<div>
  <img src="https://user-images.githubusercontent.com/34855745/53615464-52aa7180-3c21-11e9-87a6-bb4caee29fd8.JPG" width="800" height="400">
</div>

### 실제 모형 예시도
<div>
  <img src="https://user-images.githubusercontent.com/34855745/53615559-dbc1a880-3c21-11e9-9750-a0d16d54c914.JPG" width="800" height="400">
</div>

### 현장 검거시 발생하는 상황
* 경고등
<div>
  <img src="https://user-images.githubusercontent.com/34855745/53615645-407d0300-3c22-11e9-9df2-d205beedb592.JPG" width="900" height="100">
</div>
<br>

* 기록저장
<div>
  <img src="https://user-images.githubusercontent.com/34855745/53615692-820dae00-3c22-11e9-8f5d-78871b6e2bb5.JPG" width="900" height="150">
</div>
<br>

* 알림톡
<div>
  <img src="https://user-images.githubusercontent.com/34855745/53615769-dd3fa080-3c22-11e9-83c6-90760cf4c398.JPG" width="350" height="500">
</div>

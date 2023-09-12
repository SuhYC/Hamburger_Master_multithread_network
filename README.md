## Hamburger_Master_multithread_network
cpp project

## 요약
통신
- 소켓프로그래밍 사용하여 TCP 통신.

multi_thread
- thread 사용
- mutex 사용
- recursive_mutex 사용
- lock_guard 사용

stl
- vector 사용
- stack 사용
- queue 사용

## 시나리오
햄버거의 구성요소는 2가지로 한정한다. 빵(Bun)과 패티(Patty).
빵은 2가지 종류가 있다. 버터빵(Butter Bun), 참깨빵(Sesame Bun). 택1.
패티는 2가지 종류가 있다. 소고기패티(Beef Patty), 닭고기패티(Chicken Patty). 택1

클라이언트는 주문할 버거의 갯수대로 "/"로 구분하여 하나의 문자열로 만들어 서버에 전송한다.

주문서(Order)에는 주문된 버거(Burger)의 빵(Bun)과 패티(Patty) 종류와 주문한 사람의 소켓이 기록된다.

비조리된 빵과 패티는 냉장고(Refrigerator) 클래스에서 가져올 수 있다.
냉장고(Refrigerator)에서는 4가지 재료 (빵 2종류, 패티 2종류)마다 각각 Stack으로 재료를 보관하고 있으며 각각의 메소드를 통해 재료를 받아올 수 있다.
재료를 받아올 때, Stack에 저장된 재료가 없으면 생성하여 Stack에 Push하는 메소드를 5회 요청한다.

비조리된 빵과 패티는 그릴(Grill) 클래스에 있는 Queue에 넣어 조리를 시작할 수 있다.
조리를 시작하면 조리시작시간을 기록한다.
패티(Patty)는 0.15초, 빵(Bun)은 0.1초 이상 조리하면 충분히 조리된 것으로 간주한다.
패티(Patty)는 0.3초, 빵(Bun)은 0.2초 이상 조리하면 과하게 조리된 것으로 간주한다.
조리를 마치면 조리를 마친 시간을 기록하고 조리한 시간에 따라 state에 조리상태를 기록한다.

빵(Bun)과 패티(Patty) 준비가 완료되면 햄버거(Burger)를 완성한다.
완성된 햄버거(Burger)는 각각의 재료가 주문된 재료와 일치하는지와 조리상태에 따라 점수가 매겨진다.
햄버거의 종류와 점수를 문자열로 변환하여 클라이언트에 전송한다.

클라이언트는 수령한 버거를 종류마다 갯수를 기록하고 점수를 평균내어 보여준다.

## 결과
<img src="https://github.com/SuhYC/Hamburger_Master_multithread_network/blob/main/server.png" width="500"> <img src="https://github.com/SuhYC/Hamburger_Master_multithread_network/blob/main/clnt.png" width="500"><br/>

왼쪽이 서버, 오른쪽이 클라이언트.

서버측에서 받은 주문과 완성되어 배달한 주문의 순서가 다르다. 멀티스레드를 사용하였기 때문.

클라이언트에선 최종적으로 3/3/3/3개의 완성품을 정상적으로 수령했다.

<img src="https://github.com/SuhYC/Hamburger_Master_multithread_network/blob/main/clnt1.png" width="500"><br/>

갯수를 늘려 30/30/30/30도 당연히 가능

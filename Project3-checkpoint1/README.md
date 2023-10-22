# Roller Coaster

## TODO

見`doc/TODO.xlsx`

## 流程

1. `TrainView::draw`:

設定Projection Matrix -> 設定Light -> call `TrainView::drawStuff`

2. `TrainView::drawStuff`:

畫control point -> 畫軌道(`Draw::draw_track`) -> 畫火車(`Draw::draw_train`)

- `Draw::draw_track`

對於每兩個連續的control point：
> 將兩點間拆成數個段落，每個段落都以參數式求開始和結束的點，並畫上軌道和枕木。

- `Draw::draw_train`

算出火車的位置及頭頂的方向，再畫出火車。

繪製時先畫一個(-s,0,-s) ~ (s,2s,s)的立方體，再用旋轉+平移到世界座標。

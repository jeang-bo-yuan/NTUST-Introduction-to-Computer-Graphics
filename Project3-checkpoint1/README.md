# TODO

見`doc/TODO.xlsx`

# 流程

- `TrainView::drawStuff`

畫control point -> 畫軌道(`Draw::draw_track`) -> 畫火車

- `Draw::draw_track`

對於每兩個連續的control point：
> 將兩點間拆成數個段落，每個段落都以參數式求開始和結束的點，並畫上軌道和枕木。
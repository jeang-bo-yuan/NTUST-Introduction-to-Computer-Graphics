# What to Edit?
- `MazeWindow::draw`
- `Maze::Draw_View`

# Progress

|項目|狀態|附註|
|---|---|---|
|自己寫gluPerspective和gluLookAt | ***完成*** |寫在My_GLU.h|
|Clipping for one cell| ***完成*** |Frustum.h </br> `Maze::Draw_Cell` </br> `Maze::Draw_Wall_With_Clipping`|
|Clipping Recursively| ***似乎完成了***| `Frustum_2D::restrict`可用來限制視錐的範圍 </br> 偶爾會有神奇的顯示bug |
|手動轉NDC座標並繪製| ***完成*** ||

# Note

glm的mat是column-major，初始化時先指定第一欄、再指定第二欄...。

glm::make_mat*可將一維陣列轉成mat，該陣列要是column-major。
# What to Edit?
- `MazeWindow::draw`
- `Maze::Draw_View`

# Progress

|����|���A|����|
|---|---|---|
|�ۤv�ggluPerspective�MgluLookAt | ***����*** |�g�bMy_GLU.h|
|Clipping for one cell| ***����*** |Frustum.h </br> `Maze::Draw_Cell` </br> `Maze::Draw_Wall_With_Clipping`|
|Clipping Recursively| ***���G�����F***| `Frustum_2D::restrict`�i�Ψӭ�����@���d�� </br> �����|�����_�����bug |
|�����NDC�y�Ш�ø�s| ***����*** ||

# Note

glm��mat�Ocolumn-major�A��l�Ʈɥ����w�Ĥ@��B�A���w�ĤG��...�C

glm::make_mat*�i�N�@���}�C�নmat�A�Ӱ}�C�n�Ocolumn-major�C
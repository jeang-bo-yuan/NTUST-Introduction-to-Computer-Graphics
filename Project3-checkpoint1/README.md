# Roller Coaster

## TODO

��`doc/TODO.xlsx`

## �y�{

1. `TrainView::draw`:

�]�wProjection Matrix -> �]�wLight -> call `TrainView::drawStuff`

2. `TrainView::drawStuff`:

�econtrol point -> �e�y�D(`Draw::draw_track`) -> �e����(`Draw::draw_train`)

- `Draw::draw_track`

���C��ӳs��control point�G
> �N���I����ƭӬq���A�C�Ӭq�����H�ѼƦ��D�}�l�M�������I�A�õe�W�y�D�M�E��C

- `Draw::draw_train`

��X��������m���Y������V�A�A�e�X�����C

ø�s�ɥ��e�@��(-s,0,-s) ~ (s,2s,s)���ߤ���A�A�α���+������@�ɮy�СC

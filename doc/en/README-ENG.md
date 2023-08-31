# Painter GUI Application via C++ Qt Framework

### A drawing application created with the aim of mastering a wide range of features of the Qt framework.

#### At the moment, the application supports the following functionality:

- Shape modification mode
- Square creation mode
- Rectangle creation mode
- Triangle creation mode
- Circle creation mode

#### Rules defined for creating geometric shapes:

- **Square**: when the left mouse button is pressed, the center of the square is determined, when moving with the left mouse button pressed, the side of the square is determined by the distance from the center of the square to the current mouse position, divided in two. The square assumes the final state after releasing the left mouse button.
- **Rectangle**: when the left mouse button is pressed, the vertex of one of the corners of the rectangle is determined, when moving with the left mouse button pressed, the diagonal vertex of the corner of the rectangle is determined. The rectangle assumes the final state after releasing the left mouse button.
- **Triangle**: A triangle is created by three coordinates, which are determined by clicking the left mouse button sequentially. After the first click of the left mouse button and further movement of the mouse (it does not matter whether the mouse button is pressed or not) a segment is drawn from the moment of clicking to the current cursor position. It also happens after the second click, and after the third click, a triangle is created.
- **Circle**: when the left mouse button is pressed, the center of the circle is determined, when moving with the mouse button pressed, the radius of the circle is equal to the distance from the center of the circle to the current mouse position. The circle assumes its final state after releasing the left mouse button.

The process of creating all shapes is drawn dynamically.

#### Working in modification mode:

- **Selection**: In this mode, the shape is selected by clicking the left mouse button inside the shape. The selection of shapes is removed by pressing and releasing the left mouse button outside the shapes, if the coordinates of pressing and releasing coincide. If, after clicking the left mouse button outside the shape, you continue moving the mouse with the left button clamped, then a multiple selection rectangle will be drawn from the coordinates of the mouse click point to the current cursor coordinates. After releasing the left mouse button, all shapes located in the rectangle or intersecting it become highlighted. The selection is removed from all others if the _"Ctrl/Command"_ button was not pressed. If the _"Ctrl/Command"_ button was pressed, then the shapes located in the rectangle or intersecting it are added to the already selected shapes. If the _"Ctrl/Command"_ button was pressed during a single selection, then the selected figure is added to the set of already selected ones.
- **Moving**: if, after pressing the left mouse button inside one of the selected shapes, continue moving with the button held down, then all selected shapes will move a distance equal to the vector of coordinates of the left mouse button click and the coordinates of the current cursor position. The movement stops when the left mouse button is released.
- **Rotation**: if, after pressing the right mouse button inside one of the selected shapes, continue moving with the button held down, the shapes will rotate relative to their geometric center by the angle formed by the segments _OA_ and _OB_, where _O_ is the geometric center, _A_ is the cursor coordinates at the time of pressing the right mouse button, _B_ is current cursor coordinates.
- **Cloning**: Pressing the left mouse button with the _"Shift"_ button pressed inside one of the selected shapes creates copies of all selected shapes (in the same coordinates as the original shapes). If you start moving the mouse after making a copy, the copies move according to the movement rules specified above.
- **Delete**: When the _"D"_ key is pressed, all selected shapes are deleted.

## TODO:

- Add a brush drawing mode
- Add a polygon drawing mode
- Add a line drawing mode
- Add a text drawing mode
- Add a serialization/deserialization of a graphic scene using Google Protocol Buffers

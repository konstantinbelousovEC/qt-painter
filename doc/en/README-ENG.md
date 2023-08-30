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
- **Rectangle**: when the left mouse button is pressed, the vertex of one of the corners of the rectangle is determined, when moving with the left mouse button held down, the diagonal vertex of the corner of the rectangle is determined. The rectangle assumes its final state after releasing the left mouse button.
- **Triangle**: A triangle is created by three coordinates, which are determined by clicking the left mouse button sequentially. After the first click of the left mouse button and further movement of the mouse (it does not matter whether the mouse button is pressed or not) a segment is drawn from the moment of clicking to the current cursor position. It also happens after the second click, and after the third click, a triangle is created.
- **Circle**: when the left mouse button is pressed, the center of the circle is determined, when moving with the mouse button pressed, the radius of the circle is equal to the distance from the center of the circle to the current mouse position. The circle assumes its final state after releasing the left mouse button.

The process of creating all shapes is drawn dynamically.

## TODO:

- Add a brush drawing mode
- Add polygon drawing mode
- Add a line drawing mode
- Add a text drawing mode
- Add serialization/deserialization of a graphic scene using Google Protocol Buffers

# pyImageProcessing
This module provides DcimgReader class to read Hamamatsu dcimg files. This is a wrapper class of DCIMG-API for python.

## Dependencies
- DCAM-API
- DCIMG-SDK (This module uses console_dcimg.h, common_dcimg.h and common_dcimg.cpp in the samples)
- pybind11

## Build
- Microsoft Visual Studio Community 2019
- C++/17 is needed to build this module.

## Documentation
DcimgReader class represents a dcimg file.

First of all, create a `DcimgReader` Object.
```python
>>> img_file = pyImageProcessing.DcimgReader('img.dcimg')
```

To read a frame, use `read_frame(frame_index)`.
```python
>>> frame = img_file.read_frame(0)
>>> frame
array([[1723, 1667, 1677, ..., 1719, 1729, 1750],
       [1639, 1663, 1688, ..., 1705, 1763, 1711],
       [1650, 1622, 1686, ..., 1696, 1690, 1720],
       ...,
       [1668, 1661, 1670, ..., 1750, 1770, 1876],
       [1702, 1765, 1633, ..., 1719, 1689, 1687],
       [1664, 1640, 1691, ..., 1718, 1756, 1707]], dtype=uint16)
 ```
 
 To read all frames, use `read_frames(start, stop)`.
 ```python
 >>> all_frames = img_file.read_frames(0, img_file.number_of_frames)
 >>> all_frames
 array([[[1723, 1667, 1677, ..., 1719, 1729, 1750],
        [1639, 1663, 1688, ..., 1705, 1763, 1711],
        [1650, 1622, 1686, ..., 1696, 1690, 1720],
        ...,
        [1668, 1661, 1670, ..., 1750, 1770, 1876],
        [1702, 1765, 1633, ..., 1719, 1689, 1687],
        [1664, 1640, 1691, ..., 1718, 1756, 1707]],
        
        ...,
        
        [[1711, 1714, 1696, ..., 1801, 1788, 1767],
        [1736, 1636, 1697, ..., 1836, 1765, 1846],
        [1712, 1662, 1719, ..., 1849, 1788, 1843],
        ...,
        [1736, 1756, 1719, ..., 1818, 1825, 1819],
        [1759, 1700, 1655, ..., 1785, 1762, 1751],
        [1706, 1668, 1670, ..., 1832, 1757, 1781]]], dtype=uint16)
```

To get timestamps, use `get_timestamp()`.
```python
>>> timestamps = img_file.get_timestamp()
>>> timestamps
[0.0,
 0.025010108947753906,
 0.05001997947692871,
 ...]
 ```

This module provides another properties below
|Property name|Description|
|:---|:---|
|current_session|get or set a session.|
|number_of_sessions|get number of sessions in the dcimg file.|
|height|get height of the image.|
|width|get width of the image.|
|row_bytes|get rowbytes of the image.|
|bits_per_pixel|get bits per pixel of the image.|
|number_of_frames|get number of frames in current session.|

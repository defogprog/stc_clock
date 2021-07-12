# stc_clock
Alternative firmware for chinese clock kit with STC12C2052AD MCU

<details>
<summary>Picture</summary>
<p>

![Plot](./resources/clock_pic.jpg)

</p>

</details>

### Story
Once upon a time I bought a clock kit, assembled it, but it didn't work. It turned out that MCU had wrong firmware flashed (it was working, but there was some wrong activity on pis, some SPI stuff or whatever). So I decided to write my own firmware for this kit. Well, I spent 2$ - it can't go to waste :D

### Dependencies
```
sdcc, make, stcgal
```

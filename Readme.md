<div align= >
   

# <img align=center width=75px height=75px src="https://media3.giphy.com/media/OEDg5jvoZzYbBxqoRL/giphy.gif?cid=ecf05e472c40ymhxtl46b0gpa6vaexyzeqph7p3eov22ozpw&rid=giphy.gif&ct=s"> Operating System Scheduler


</div>
<p align="center">
   <img align="center" width=600px height=450px src="https://cdn.dribbble.com/users/1555712/screenshots/5356776/microsoft.gif" alt="logo">
</p>

<p align="center"> 
    <br> 
</p>

## <img align= center width=50px height=50px src="https://thumbs.gfycat.com/HeftyDescriptiveChimneyswift-size_restricted.gif"> Table of Contents

- <a href ="#about"> 📙 overview</a>
- <a href ="#Started"> 💻 Get Started</a>
- <a href ="#Path"> 📷 Path of the program</a>
- <a href ="#Assumptions"> 📃 Assumptions</a>
- <a href ="#Structure"> 📁  File Structure</a>
- <a href ="#Contributors"> ✨ Contributors</a> 
- <a href ="#License"> 🔒 License</a> 
<hr style="background-color: #4b4c60"></hr>

 <a id = "about"></a>

## <img align="center"  width =60px  height =70px src="https://media2.giphy.com/media/Yn4nioYWSZMqiPNVuD/giphy.gif?cid=ecf05e47m5h78yoqhdkg8pq54o5qsxhdoltjxyfe08d4vxvg&rid=giphy.gif&ct=s"> Overview
<br>
<ul> 
<li>
A CPU scheduler determines an order for the execution of its scheduled processes.</li>
<li>it decides which process will run according to a certain data structure that keeps track of the processes in the system and their status.</li>
<li>A process, upon creation, has one of the three states: Running, Ready, Blocked (doing I/O, using other resources than CPU or waiting on unavailable resource).</li>
<li>
A bad scheduler will make a very bad operating system, so your scheduler should be as much optimized as possible in terms of memory and time usage.
</li>
</li>

<li>
The project has 2 phases

 - <a href="https://github.com/AdhamAliAbdelAal/OS-Project/blob/master/Project%20Phase%201.pdf">Project Phase 1</a>
 - <a href="https://github.com/AdhamAliAbdelAal/OS-Project/blob/master/Project%20Phase%202.pdf">Project Phase 2</a>
</li>
</ul>
<hr style="background-color: #4b4c60"></hr>
<a id = "Started"></a>

## <img  align= center width=50px height=50px src="https://c.tenor.com/HgX89Yku5V4AAAAi/to-the-moon.gif"> Get Started 

<ol>
<li>Clone the repository

<br>

```
git clone https://github.com/AdhamAliAbdelAal/OS-Project
```

</li>
<li> You will need to download platform <a href="https://www.linux.org/">Linux</a> </li>
</ol>
<hr style="background-color: #4b4c60"></hr>


## <img align= "center" width=80px height=80px src="https://media2.giphy.com/media/dAu3qBzpmXstCTimDv/giphy.gif?cid=ecf05e475rq4hwxg7usfm1go2dc1pl5gpbnvo1izu6ph4yuh&rid=giphy.gif&ct=s"> Path of the program <a id ="Path"></a>

<hr style="background-color: #4b4c60"></hr>
 <a id ="Assumptions"></a>

## <img align= "center" width=60px height=70px src="https://media2.giphy.com/media/8pEnqbR2gapFekW4KK/giphy.gif?cid=ecf05e47ire2dp6wrcli5orn0gddraxve7sug4v3753pquxa&rid=giphy.gif&ct=s">Assumptions
<br>
<ol>
<li>
In the memory waiting queue, it is implemented as a priority queue based on the algorithm so if it’s a SRTN, the priority is the remaining time while for RR, it is based on the memory size where the smaller one gets put into the ready queue first. As for the HPF, there is no need since the running process is the only one put into the ready queue.
</li>
<br>
<li>
We made an array of arrivals in the process generator as a shared memory with the scheduler in order to make sure that any process arriving at a specific time is read by the scheduler and not skipped.
</li>
<br>
<li>We synchronize between the stopped process and the arrived one if they come in the same second so that the stopped process is put into the queue before the arrived process.
</li>
</ol>

<hr style="background-color: #4b4c60"></hr>
<a id="Structure"> </a>

## <img align= center width=60px height=60px src="https://media1.giphy.com/media/igsIZv3VwIIlRIpq5G/giphy.gif?cid=ecf05e47faatmwdhcst7c2d4eontr459hjd35zf3an324elo&rid=giphy.gif&ct=s"> File Structure 
<br>
<div align= center>
<img   src="https://user-images.githubusercontent.com/71986226/182103221-e5d5b882-f846-4794-814f-5f42403948a8.png">
</div>

<hr style="background-color: #4b4c60"></hr>

## <img  align="center" width= 70px height =55px src="https://media0.giphy.com/media/Xy702eMOiGGPzk4Zkd/giphy.gif?cid=ecf05e475vmf48k83bvzye3w2m2xl03iyem3tkuw2krpkb7k&rid=giphy.gif&ct=s"> Contributors <a id ="Contributors"></a>

<table align="center" >
  <tr>
     <td align="center"><a href="https://github.com/AbdelrahmanNoaman"><img src="https://avatars.githubusercontent.com/u/76150639?v=4" width="200px;" alt=""/><br /><sub><b>Abdelrahman Noaman</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/abdelrahman0123"><img src="https://avatars.githubusercontent.com/u/67989900?v=4" width="200px;" alt=""/><br /><sub><b>Abdelrahman Hamdy</b></sub></a><br /></td>
     <td align="center"><a href="https://github.com/AdhamAliAbdelAal" ><img src="https://avatars.githubusercontent.com/u/83884426?v=4" width="200px;" alt=""/><br /><sub><b>Adham Ali</b></sub></a><br />
    </td>
     <td align="center"><a href="https://github.com/EslamAsHhraf"><img src="https://avatars.githubusercontent.com/u/71986226?v=4" width="200px;" alt=""/><br /><sub><b>Eslam Ashraf</b></sub></a><br /></td>
  </tr>
</table>

## 🔒 License <a id ="License"></a>

>This software is licensed under MIT License, See [License](https://github.com/AdhamAliAbdelAal/MP-Processor-Game/blob/master/LICENSE) for more information ©AdhamAliAbdelAal.

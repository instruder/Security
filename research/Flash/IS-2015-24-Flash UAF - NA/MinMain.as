package
{
import flash.display.*;
import flash.media.*;
import flash.geom.*;
import flash.system.*;
import adobe.utils.*;
import flash.utils.ByteArray;


public class Main extends Sprite
{
var main_obj:Video;
public var worker:Worker;

public function Main() 
{
if (Worker.current.isPrimordial) mainThread();
else workerThread();
}

private function mainThread():void
{
main_obj =new Video(10,0xFF0000);//large memory
this.addChild(main_obj);

var local3:Vector3D =new Vector3D(0,10,0x1FFFF,0);
local3.y = 63;

main_obj.local3DToGlobal(local3);

worker = WorkerDomain.current.createWorker(this.loaderInfo.bytes)
worker.setSharedProperty("main_obj", main_obj)
worker.start()
}

private function workerThread():void
{
	
var debug:ByteArray = new ByteArray();
var main_obj:Video = Worker.current.getSharedProperty("main_obj");
var local12:Point = new Point(0x199FF, 0x10000);
main_obj.globalToLocal(local12);

}

}

}

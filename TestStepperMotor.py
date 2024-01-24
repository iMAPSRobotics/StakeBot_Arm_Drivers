
import RobotControls as RC

# Hardware Peripherals #

stepDriver = ["MainArm"]
stepDev = ["/dev/ttyACM0"] ## <--- MAKE SURE YOU CHANGE ME TO THE CORRECT SERIAL PORT FOR WINDOWS USE 'COM#'
baud_rate = 115200

#########################

## Main Function ##

if __name__ == "__main__":
    
    ### Threads ###

    s = RC.StakeBotMotorDrivers(stepDriver, stepDev, baud_rate)
  
    print("Welcome to the stepper driver test for the stakebot robot!")
    while True:
        cmd = input("Type 'calibrate' to calibrate\nType 'getPosition' with either 'motorA' or 'motorB' to get position\nType 'measureBounds' with either 'motorA' or 'motorB' to start measuring distance\nType 'goToPosition' with (motorA position , motorB position) '10,-10' to go to position\nType 'stop' to stop\n")
        if cmd != "calibrate":
            if cmd != "stop":
                dir = input("Give directions ")
                if cmd == "getPosition":
                    s.getPosition(dir)
                elif cmd == "measureBounds":
                    s.measureBounds(dir)
                elif cmd == "goToPosition":
                    dir = dir.split(',')
                    s.goToPosition(dir[0], dir[1])
                else: 
                    print("Invalid Selection")
            else:
                s.stop()
        else:
            s.calibrate()
        print("\nPositionA: {0}".format(s.posA))
        print("PositionB: {0}".format(s.posB))
        print("BoundsA: {0}".format(s.boundsA))
        print("BoundsB: {0}\n".format(s.boundsB))    
        

    
        


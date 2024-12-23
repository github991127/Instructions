 	<!--界关兴张苞 同心并力-->
	<!--使用界关兴张苞在一局游戏中通过父魂使用两种颜色的转化杀-->
	<Achievement Id="1305" category="67108880" point="5"  unlockAchievement="0" unlockLevel="0" characterId="690" figure="0" gameMode="2" flags="1024" desc="使用界关兴张苞在一局游戏中通过父魂使用两种颜色的转化杀。" requirementCount="0">
	    <CriteriaEntry Id="1823" count="1" flag="4" desc="父魂使用红色杀">
	      <Event eventType="78" type="0" compare1="1" value1="3651" compare2="1" value2="1" recordSeatType="2">
	      </Event>
	      <Event eventType="1" type="1">游戏开始
	      </Event>
	      <Event eventType="2" type="1">游戏结束
	      </Event>
	    </CriteriaEntry>
	    <CriteriaEntry Id="1824" count="1" flag="4" desc="父魂使用黑色杀">
	      <Event eventType="78" type="0" compare1="1" value1="3651" compare2="1" value2="2" recordSeatType="2">
	      </Event>
	      <Event eventType="1" type="1">游戏开始
	      </Event>
	      <Event eventType="2" type="1">游戏结束
	      </Event>
	    </CriteriaEntry>
    </Achievement>

    <!--界关兴张苞 父魂子魄-->
	<!--使用界关兴张苞获得100场身份模式游戏胜利-->
	<Achievement Id="1306" category="67108880" point="45" unlockAchievement="0" unlockLevel="0" characterId="690" figure="0" gameMode="2" flags="2048" requirementCount="0">
		<CriteriaEntry Id="1825" count="100" flag="0" desc="{0}场游戏胜利">
			<Event eventType="4" type="0">牌局胜利
       		</Event>
			<Segment count="15" point="10"/>
			<Segment count="50" point="25"/> 
			<Segment count="100" point="45"/>
		</CriteriaEntry>
	</Achievement>

 	<!--界马岱 我敢杀你-->
	<!--使用界马岱在一局游戏中使用潜袭牌杀死一名角色并获胜-->
	<Achievement Id="1317" category="67108880" point="5"  unlockAchievement="0" unlockLevel="0" characterId="696" figure="0" gameMode="2" flags="1024" desc="使用界马岱在一局游戏中使用潜袭牌杀死一名角色并获胜。" requirementCount="0">
   		<CriteriaEntry Id="1826" count="1" flag="4" desc="击杀角色">
		<Event eventType="7" type="0" compare1="1" value1="3663" compare2="1" value2="1" >
		</Event>
      	<Event eventType="1" type="1">游戏开始
      	</Event>
      	<Event eventType="2" type="1">游戏结束
      	</Event>
    	</CriteriaEntry>
  </Achievement> 

    <!--界马岱 狮宗继血-->
	<!--使用界马岱获得100场身份模式游戏胜利-->
	<Achievement Id="1318" category="67108880" point="45" unlockAchievement="0" unlockLevel="0" characterId="696" figure="0" gameMode="2" flags="2048" requirementCount="0">
		<CriteriaEntry Id="1827" count="100" flag="0" desc="{0}场游戏胜利">
			<Event eventType="4" type="0">牌局胜利
       		</Event>
			<Segment count="15" point="10"/>
			<Segment count="50" point="25"/> 
			<Segment count="100" point="45"/>
		</CriteriaEntry>
	</Achievement>

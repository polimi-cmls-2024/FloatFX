//SynthDefs - Kick, Sin, Saw, Triangle, Pulse

//Pseq, Sequentially output values from a list

// Prand, Randomly output values from a list

//Pxrand, Randomly output values from a list but never the same item twice in a row.

//Pexprand, Randomly chooses from an exponential distribution of values

//Pwrand - Randomly based on a array of weights

//Pwhite - returns random from (lo: , hi: , length: )

//Pbind - A sequence of pairs of values. Used to make events.

//Pgeom - Multiplicative. (20000, 0.5, 6) 20000 will be cut in half iteratively, 6 times.

//Pseries - Adds a number to another, n times. For eg: (50, 7, 6) -  adds 7 to 50, 6 times.

s.boot

(
SynthDef(\kick, {
	var sig, env, freqSweep;
	env = Env([0, 1, 0], [0.005, 0.5], [1, -1]).ar(doneAction: 2);
	freqSweep = Env([2000, 200, 10], [0.005, 0.5], [1,-12]).ar;
	sig = {SinOsc.ar(freqSweep) *env };
	sig = sig.distort * 1.5;
	sig = Pan2.ar(sig * 2, \pan.ir(0), \amp.ar(0.5));
	Out.ar(\out.ar(0), sig);
}
).add;


SynthDef(\hats, {
	var sig, env;
	env = Env.perc(attackTime: 0.05, releaseTime: 0.05, curve: -8).ar(doneAction: 2);
	sig = {PinkNoise.ar(0.7) *env };
	sig = HPF.ar(sig, \hpf.ir(1000).clip(20, 20000));
	sig = Pan2.ar(sig * 2, \pan.ir(0), \amp.ar(0.5));
	Out.ar(\out.ar(0), sig);
}
).add;


SynthDef(\sin, {
	var sig, env;
	sig = SinOsc.ar(\freq.ir(1000) * 2.pow({(\dtun.ir(0.05)/12).sum3rand} ! 10)); //Detuning with a default of 0.05 and splitting into 10 voices
	env = Env.linen(\atk.ir(0.001), \sus.ir(0.3), \dec.ir(0.1), curve: [-1, 0, -6]).ar(doneAction: 2);
	sig = LPF.ar(sig, \lpf.ir(20000).clip(20, 20000));
	sig = Splay.ar(sig); //Spread it across the stereo field.
	sig = Balance2.ar(sig[0], sig[1], \pan.ir(0), \amp.ar(0.5)) * env;
	Out.ar(\out.ar(0), sig);
	}
).add;



SynthDef(\saw, {
	var sig, env;
	sig = Saw.ar(\freq.ir(1000) * 2.pow({(\dtun.ir(0.05)/12).sum3rand} ! 10)); //Detuning with a default of 0.05 and splitting into 10 voices
	env = Env.linen(\atk.ir(0.001), \sus.ir(0.3), \dec.ir(0.1), curve: [-1, 0, -6]).ar(doneAction: 2);
	sig = LPF.ar(sig, \lpf.ir(20000).clip(20, 20000));
	sig = Splay.ar(sig); //Spread it across the stereo field.
	sig = Balance2.ar(sig[0], sig[1], \pan.ir(0), \amp.ar(0.5)) * env;
	Out.ar(\out.ar(0), sig);
	}
).add;



SynthDef(\tri, {
	var sig, env;
	sig = LFTri.ar(\freq.ir(1000) * 2.pow({(\dtun.ir(0.05)/12).sum3rand} ! 10)); //Detuning with a default of 0.05 and splitting into 10 voices
	env = Env.linen(\atk.ir(0.001), \sus.ir(0.3), \dec.ir(0.1), curve: [-1, 0, -6]).ar(doneAction: 2);
	sig = LPF.ar(sig, \lpf.ir(20000).clip(20, 20000));
	sig = Splay.ar(sig); //Spread it across the stereo field.
	sig = Balance2.ar(sig[0], sig[1], \pan.ir(0), \amp.ar(0.5)) * env;
	Out.ar(\out.ar(0), sig);
	}
).add;



SynthDef(\pul, {
	var sig, env;
	sig = Pulse.ar(\freq.ir(1000) * 2.pow({(\dtun.ir(0.05)/12).sum3rand} ! 10), width: \wid.ir(0.6)); //Detuning with a default of 0.05 and splitting into 10 voices
	env = Env.linen(\atk.ir(0.001), \sus.ir(0.3), \dec.ir(0.1), curve: [-1, 0, -6]).ar(doneAction: 2);
	sig = LPF.ar(sig, \lpf.ir(20000).clip(20, 20000));
	sig = Splay.ar(sig); //Spread it across the stereo field.
	sig = Balance2.ar(sig[0], sig[1], \pan.ir(0), \amp.ar(0.5)) * env;
	Out.ar(\out.ar(0), sig);
	}
).add;

/*SynthDef(\arp, {
	arg tempo = 4;
	var sig;
	sig = SinOsc.ar{
		(\freq.ir(1000) * LFPulse.kr(freq: [tempo, tempo+1], width: 0.3))};
	sig = sig * \amp.ir(0.4);
	Out.ar(\out.ar(0), sig);
}).add;
		*/


SynthDef(\rain, {
	arg freq;
	var sig, env;
	sig = PinkNoise.ar(0.1!2); //amplitude of 0.1 (mul) in 2 channels L and R.
	sig = RLPF.ar(sig, freq: 500, rq: 1);
	env = SinOsc.kr(1, 2*pi);
	sig = sig * env;
	Out.ar(\out.ar(0), sig);
}).add;

)

//test the synths here. Only for testing purpose, not used during playback.

Synth(\kick)

Synth(\sin)

Synth(\saw)

Synth(\tri)

Synth(\pul)

Synth(\rain)

/*Synth(\arp)*/


//ProxySpace initialisation

(
p = ProxySpace(s, clock: TempoClock(100/60)).fadeTime_(3).quant_(4).push;

~kick.play;
~dots.play;
~hats.play;
~pads.play;
~arp.play;
~rain.play;
)

//Playback

(
(
~kick = Pbind(

	\instrument, \kick,
	//\dur, Pxrand([0, 0.25, 0.5, 1], inf)
	\dur, Pseq([1, 0, 1, 0,], inf),
	\db, -30.clip(-30, -15),
			)
);

(
~hats = Pbind(
	\instrument, \hats,
	\dur, Pxrand([0, 0.25, 0.5, 1], inf).round(0.25),
	//\dur, Pseq([0.25], inf),
	\hpf, Pexprand(1000, 20000, inf),
	\pan, Pwhite(-1.0, 1.0),
)
);



(
~dots = Pbind(
	\instrument, Prand([\saw, \sin, \pul, \tri],inf),
	\degree, Pxrand([0, 2, 3, 4, 5], inf) + Prand([-14, -7, 0, 7, 14], inf),
	\db, -35.clip(-35,-20),
	\pan, Pwhite(-1.0, 1.0),
	\dur, Pexprand(0.01, 1.0, inf).round(0.25),
	\atk, Pexprand(0.001, 0.1),
	\dec, Pexprand(0.001, 0.9),
	\sus, Pexprand(0.1, 0.4),
	//\lpf, 5000,
)

);

(
~pads = Pbind(
	\instrument, \saw,
	\degree, Pxrand([Pxrand([0, 2, 4, 7, 9, 11], 1), Pxrand([0, 2, 5, 7, 9, 12], 1), Pxrand([-1, 1, 4, 6, 8, 11], 1), Pxrand([0, 3, 5, 7, 10, 14], 1)],inf), //Picks notes from Cmaj, Amin, Gmaj, Fmaj picked at random, played infinitely
	\lpf, 200,
	\mtranspose, -7,
	\atk, 8,
	\rel, 8,
)
);

(
~arp = Pbind(
	\instrument, \tri,
	\degree, Pseq([0], inf),
	\hpf, 500,
	\sus, 0.1,
	\db, -30,
	\pan, Pwhite(-1.0, 1.0),
	\dur, 0.25,
)
);

)
// (
// ~rain = Pbind(
// \instrument, \rain)
// )

//Stop Playback
(
~kick.stop;
~dots.stop;
~hats.stop;
~arp.stop;
~pads.stop;
~rain.stop;
)

s.boot;

(
SynthDef(\saw, {
	var sig, buf = \buf.ir(0);
	sig = Saw.ar(\freq.ir(200), mul: 0.5);
	sig = RLPF.ar(in: sig, freq: \lpf.ir(20000).clip(20,20000),rq: \rq.ir(1));
	sig = sig * Env.adsr().ar(2, \gate.kr(1));
	sig = Pan2.ar(sig, \pan.ir(0), \amp.ir(0.2)); // For sending the signal to both l and r.
	Out.ar(\out.ir(0), sig);
}).add;
)


(
~pat = Pbind(
	\instrument, \saw,
//	\dur, 0.2, //duration between 2 notes in beats
	\dur, Prand([0.2, 0.4, 0.6], inf),
	\amp, 0.4,
	\sustain, 0.2,
	\midinote, Pwhite(40,80,inf)
);

~seq = ~pat.play;
)
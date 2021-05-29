import { AfterViewInit, Component, OnInit, ViewChild } from '@angular/core';
import { NgTerminal, DisplayOption } from 'ng-terminal';
import { ReplService } from '../repl.service';
import { Event } from '../event'

@Component({
  selector: 'app-repl',
  templateUrl: './repl.component.html',
  styleUrls: ['./repl.component.css']
})
export class ReplComponent implements OnInit, AfterViewInit {
  //@ViewChild('term', { static: true }) child: NgTerminal;
  @ViewChild('term', { static: true }) child!: NgTerminal;

  constructor(private replService: ReplService ) { }

  ngOnInit(): void {
  }

  ngAfterViewInit(): void{
    const opts: DisplayOption = { "fixedGrid": { cols: 40, rows: 20 } };
    this.child.setDisplayOption(opts);
    this.child.keyEventInput.subscribe(e => {
      console.log('keyboard event:' + e.domEvent.keyCode + ', ' + e.key);

      const ev = e.domEvent;
      const printable = !ev.altKey && !ev.ctrlKey && !ev.metaKey;

      if (ev.keyCode === 13) {
        this.child.write('\r\n');
        const line = this.child.underlying.buffer.active.getLine(this.child.underlying.buffer.active.cursorY);
        if (line)
        {
          console.log('current line: ' + line.translateToString(false, 2))
          this.replService.newLine(line.translateToString(false, 2))
        }
      } else if (ev.keyCode === 8) {
        if (this.child.underlying.buffer.active.cursorX > 2) {
          this.child.write('\b \b');
        }
      } else if (printable) {
        this.child.write(e.key);
      }
    })
    
    //
    this.replService.getServerSentEvent().subscribe(data => {
      const ev: Event = JSON.parse(data);
      if (ev.event === 'ReplResponseEvent')
      {
        this.child.write(ev.data);
      }
    });
  }

}

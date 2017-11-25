
r1 <- list(x = 972.615, y = 18.9850, w = 50, h = 6)
r2 <- list(x = 971.471, y = 18.9657, w = 1, h = 37.93)

ggplot() + 
  annotate('rect', xmin = r1$x - r1$w/2, ymin = r1$y - r1$h/2, xmax = r1$x + r1$w/2, ymax = r1$y + r1$h/2, color = 'grey', alpha = 0.2) +
  annotate('rect', xmin = r2$x - r2$w/2, ymin = r2$y - r2$h/2, xmax = r2$x + r2$w/2, ymax = r2$y + r2$h/2, color = 'grey', alpha = 0.2)